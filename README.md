## Setting
* Linux OS (Ubuntu)
* g++ 5.5.0
* Python 3.8.8

## Datasets
* Vectors in an inner product space
    * We used [MF](https://www.csie.ntu.edu.tw/~cjlin/libmf/) on sets of rating data.
    * Place datasets in `dataset/dataset_MF200`.

* Vectors in a metric space
    * We used codes in `item2vec` directory to make the vectors.
    * Place datasets in  `dataset/dataset_item2vec`.

## Algorithms
* Parameters are read from `parameter` directory (e.g., k.txt & lamda.txt).
* See each algorithm directory to see how to run.