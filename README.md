# Fast Center-of-mass Computation for EDF Stream

## Compilation

```sh
git clone https://github.com/blochl/edfCM.git
cd edfCM
make
```

## Usage

pipe stdout to: `./edfCM NUMIMAGES`

Example (in this case each file contains one image):
```sh
cat file1.edf file2.edf file3.edf | ./edfCM 3
```

* If `NUMIMAGES` is more than the number of streamed images - the job will be terminated when the stream will end.
* If `NUMIMAGES` is less than the number of streamed images - only `NUMIMAGES` images will be processed.
* The images can be in one .edf file, several files, a continuous stream, or mix of the above.
