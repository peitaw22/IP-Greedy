## Introduction
* This repository provides an implementation of IP-Greedy.
* This is a efficient and effective algorithm for the Diversity-Aware Maximum Inner Product Search.
* As for the details about IP-Greedy, please read our RecSys2022 paper,  
Solving Diversity-Aware Maximum Inner Product Search Efficiently and Effectively



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

## IP-Greedy
* Parameters are read from `parameter` directory (e.g., k.txt & lamda.txt).
* See IP-Greedy directory to see how to run.

## Citation
If you use our implementation, please cite the following paper.
```
@inproceedings{?,  
    title={Solving Diversity-Aware Maximum Inner Product Search Efficiently and Effectively},  
    author={Hirata, Kohei and Amagata, Daichi and Hara, Takahiro and Fujita, Sumio},  
    booktitle={RecSys},  
    pages={?--?},  
    year={2022}  
}
```

## License
Copyright (c) 2022 peitaw22  
This software is released under the [MIT license](https://github.com/peitaw22/IP-Greedy/blob/main/license.txt).