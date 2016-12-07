/************************************************
* Written by Leonid Bloch (c) 2016
* Distributed under GPLv2
************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define READBYTES_DAT_MIN  128 /* Def. value is 128, must be divisible by 4  */
#define READBYTES_HDR      128 /* Hdr must be divisible by it (must for EDF) */
#define MAX_HEAD_SIZE      8192
#define S_SIZE             sizeof("Size =")
#define S_DIM              sizeof("Dim_X =")
#define MIN(a,b)           (((a) < (b)) ? (a) : (b))

typedef struct rec
{
    uint64_t x;
    uint64_t y;
    uint64_t v;
} rec;

typedef struct meta
{
    int Dim_1;
    int Dim_2;
    int depth;
    int headlen;
} meta;

typedef union aval
{
    char *av_1;
    uint16_t *av_2;
    uint32_t *av_4;
} aval;

static inline void
parsehead(FILE *p_str, aval **curr_aval, meta *meta_curr, int *readbytes)
{
    char hdr[MAX_HEAD_SIZE] = {0};
    int headlen = 0;
    do {
        fread(&hdr[headlen * READBYTES_HDR], 1, READBYTES_HDR, p_str);
        headlen++;
    } while( hdr[headlen * READBYTES_HDR - 2] != '}' );
    int size = atoi(strstr(hdr, "Size") + S_SIZE);
    int dim1 = atoi(strstr(hdr, "Dim_1") + S_DIM);
    int dim2 = atoi(strstr(hdr, "Dim_2") + S_DIM);
    meta_curr->Dim_1 = dim1;
    meta_curr->Dim_2 = dim2;
    meta_curr->depth = size / dim1 / dim2;
    meta_curr->headlen = headlen * READBYTES_HDR;
    if ( meta_curr->depth != 1 && meta_curr->depth != 2
         && meta_curr->depth != 4 ) {
        fprintf(stderr, "Depth must be only 1, 2, or 4, but got: %d\n",
                meta_curr->depth);
        exit(1);
    }
    readbytes[0] = MIN(READBYTES_DAT_MIN, dim1 * meta_curr->depth);
    while ( (dim1 * meta_curr->depth) % readbytes[0] ) {
        readbytes[0] += meta_curr->depth;
    }
    *curr_aval = malloc(readbytes[0]);
    (*curr_aval)->av_1 = malloc(readbytes[0]);
}

static inline void
skiphead(FILE *p_str, const meta *meta_curr)
{
    static char buf[READBYTES_HDR];
    fread(&buf[0], 1, READBYTES_HDR, p_str);
    if ( buf[0] != '{' ) {
        exit(0);
    }
    int i;
    for ( i = 1; i < meta_curr->headlen / READBYTES_HDR; i++ )
    {
        fread(&buf[0], 1, READBYTES_HDR, p_str);
    }
}

static inline void
calc(FILE *p_str, aval *curr_aval, const meta *meta_curr, const int *readbytes)
{
    int i, j, k;
    double x_c, y_c;
    const int step = readbytes[0] / meta_curr->depth;
    const int dim_1_rb = meta_curr->Dim_1 / step;
    uint64_t tval = 0;
    rec curr_rec = {0};
    for ( i = 1; i <= meta_curr->Dim_2; i++ )
    {
        for ( j = 0; j < dim_1_rb; j++ )
        {
            fread(&curr_aval->av_1[0], 1, readbytes[0], p_str);
            for ( k = 0; k < step; k++ )
            {
                switch ( meta_curr->depth ) {
                    case 1:
                        curr_rec.x += (j * step + k + 1) * curr_aval->av_1[k];
                        tval += curr_aval->av_1[k];
                    break;
                    case 2:
                        curr_rec.x += (j * step + k + 1) * curr_aval->av_2[k];
                        tval += curr_aval->av_2[k];
                    break;
                    case 4:
                        curr_rec.x += (j * step + k + 1) * curr_aval->av_4[k];
                        tval += curr_aval->av_4[k];
                    break;
                }
            }
        }
        curr_rec.y += i * tval;
        curr_rec.v += tval;
        tval = 0;
    }
    if ( curr_rec.v ) {
        x_c = (double)curr_rec.x / curr_rec.v - 1;
        y_c = (double)curr_rec.y / curr_rec.v - 1;
    } else {
        x_c = (double)meta_curr->Dim_1 / 2;
        y_c = (double)meta_curr->Dim_2 / 2;
    }
    printf("%.6f,%.6f\n", x_c, y_c);
}

int main( int argc, char *argv[] )
{
    if ( argc != 2 ) {
        printf("\nFAST CENTER-OF-MASS COMPUTATION FOR EDF STREAM\n\n"
               "Usage - pipe stdout to: %1$s NUMIMAGES\n\n"
               "Example (in this case each file contains one image):\n"
               "cat file1.edf file2.edf file3.edf | %1$s 3\n\n"
               "If NUMIMAGES is more than the number of streamed images -\n"
               "  - the job will be terminated when the stream will end.\n"
               "If NUMIMAGES is less than the number of streamed images -\n"
               "  - only NUMIMAGES images will be processed.\n\n"
               "The images can be in one .edf file, or several.\n\n", argv[0]);
        return 1;
    }
    static int readbytes[1];
    meta *meta_curr = malloc(sizeof(meta));
    aval *curr_aval;
    int g = atoi(argv[1]);
    parsehead(stdin, &curr_aval, meta_curr, readbytes);
    while(g)
    {
        calc(stdin, curr_aval, meta_curr, readbytes);
        switch ( g ) {
            case 1:
            break;
            default:
                skiphead(stdin, meta_curr);
        }
        g--;
    }
    free(meta_curr);
    free(curr_aval->av_1);
    free(curr_aval);
    return 0;
}
