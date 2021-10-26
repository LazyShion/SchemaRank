## Fast ObjectRank for Large Knowledge Databases
The implementation of SchemaRank proposed in ISWC'21.
If you use our software, **please cite the following paper**.

```
Hiroaki Shiokawa,  
"Fast ObjectRank for Large Knowledge Databases,"
Proceedings of the 20th International Semantic Web Conference (ISWC2021), pp.217-234, October, 2021.
```
``` 
@inproceedings{Shiokawa2021b,
  title   = {{Fast ObjectRank for Large Knowledge Databases}},
  author  = {Hiroaki Shiokawa},
  booktitle = {The Semantic Web -- ISWC 2021},
  pages   = {217--234},
  year    = {2021},
}
```

### LICENSE
This software is released under the MIT license. Please read `LICENSE` for details.


## How to Use
### Requirements
SchemaRank requires the following softwares.
* gcc Version 9.2.0 (or later)

We have confirmed that our software works on the following environments.
* CentOS 8
* MacOS 11.6

### Build
1. Install *g++* on your environment.
2. Build the softwares in `./generate_crs` and `./`.
```
$ cd generate_crs
$ make clean; make
$ cd ..
$ make clean; make
```
3. If you can find `schemarank` and `gen_crs` in `./` and `./generate_crs`, respectively, the build has been successfully completed.


### Usage
#### Input files
SchemaRank requires three input files: `schema_graph.txt`, `data_graph.txt`, and `query.txt`.

`schema_graph.txt` is a file that represents a schema graph of a given knowledge graph.
The file must include a list of adjacent node-types in the schema graph with a corresponding edge-weight value like as follows:

``` schema_graph.txt
A	Ps	0.2
Ps	A	0.2
Ps	Pd	0.7
Ps	Y	0.1
Y	Ps	0.3
Y	C	0.3
C	Y	0.3
```
For example, if node-types `A` and `Ps` are connected with a weighted edge with 0.2, `schema_graph.txt` should have a line of `A\tPs\t0.2`, where `\t` is TAB.

`data_graph` is a file that represents a data graph of a given knowledge graph.
The file must include a list of adjacent nodes in the data graph along with a corresponding edge weitht value and node-types.

``` data_graph.txt
0	1	0.15	CY
0	2	0.15	CY
1	0	0.3	YC
1	3	0.3	YPs
2	0	0.3	YC
2	4	0.3	YPs
3	1	0.1	PsY
3	4	0.7	PsPd
3	5	0.2	PsA
4	2	0.1	PsY
4	5	0.1	PsA
4	6	0.1	PsA
5	3	0.1	APs
5	4	0.1	APs
6	4	0.2	APs
```
For example, if a data graph has an edge between node-id `0` and `1` with a weight value 0.15, and `0` and `1` have node-types `C` and `Y`, respectively, this file should contain a line of `0\t1\t0.15\tCY`, where `\t` is TAB.

`query.txt` is a file that specifies query nodes included in `data_graph.txt`.
Specifically, if we select node-ids `0` and `1` as query nodes, `query.txt` should be as follows:

``` query.txt
0
1
```


#### File conversion
SchemaRank reads the given files by using the CRS format, and this thus requires a file conversion process. 
To covert the input files (`schema_graph.txt` and `data_graph.txt`) into the CRS format (`output_sg.bin` and `output_dg.bin`), we need to run `gen_crs` like as follows:
``` convert
$ cd ./generate_crs/
$ ./gen_crs -s schema_graph.txt -d data_graph.txt
```
`gen_crs` requires two options, `-s` and `-d`, that specify `schema_graph.txt` and `data_graph.txt`, respectively.


#### Run SchemaRank
Finally, we can run SchemaRank like as follows:
```
$ ./schemarank -i <sg_crs_file> <dg_crs_file> -q query.txt 
```
where `<sg_crs_file>` and `<dg_crs_file>` are CRS files of a schema and a data graph obtained by `generate_crs`, respectively.

To learn further options of `schemarank`, please run `./schemarank -h`.


## Other reference
* http://www.kde.cs.tsukuba.ac.jp/~shion/index.html.en
