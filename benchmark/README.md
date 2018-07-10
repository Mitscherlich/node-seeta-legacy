node-seeta benchmark
---

This directory contains benchmarking schemas. We follow the [seetaface/SeetaFaceEngine](https://github.com/seetaface/SeetaFaceEngine#description)'s document and use [LFW](http://vis-www.cs.umass.edu/lfw/)(Labeled Faces in the Wild) database for testing. For more infomation, you can read these two pagers http://arxiv.org/abs/1609.07304 https://people.cs.umass.edu/~elm/papers/LFW_survey.pdf.

## Prerequisite

First, you need to follow the instruction in the root directory's README to build this addon, then:

### Big data

Since LFW dataset is not included in the directory initially, you need to run the following command to download the testing data:

```bash
$ ./download_data.sh
```

After doing this the big data file will automaticly generated in the benchmark directory.

## Run instructions

To run all the benchmark dataset:

```bash
$ BENMARK=[Detection|Alignment] npm run benchmark # name a schema for benchmark type.
```

## Benchmark datasets

All face database in image form are supported.

We would like to add more data sets. In general we will favor data sets that make the overall suite diverse without being too large or having too many similar tests. Ideally everyone can run through the entire suite without the test run getting too long.
