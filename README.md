# Fast Center-of-mass Computation for EDF Stream

## Compilation

```sh
git clone https://github.com/blochl/edfCM.git
cd edfCM
make
```

## Usage

Pipe stdout to: `./edfCM NUMIMAGES`

* If `NUMIMAGES` is more than the number of streamed images - the job will be terminated when the stream will end.
* If `NUMIMAGES` is less than the number of streamed images - only `NUMIMAGES` images will be processed.
* The images can be in one .edf file, several files, a continuous stream, or mix of the above.

### Example

In this case each file contains one image:

```sh
cat file1.edf file2.edf file3.edf | ./edfCM 3
```

### Plotting

There is an option to plot the results, using the auxiliary **edfCMp** script.
An installation of [gnuplot](http://gnuplot.sourceforge.net/) is required for
that.

#### Examples

* To plot, pipe the output of **edfCM** to **edfCMp**:

```sh
cat file1.edf file2.edf file3.edf | ./edfCM 3 | ./edfCMp
```

* To still get the output in the terminal window and simultaneously get the plot,
do:

```sh
cat file1.edf file2.edf file3.edf | ./edfCM 3 | tee >(./edfCMp)
```

## Licensing

Distributed under GPLv2.
