# SC3020-project-2

## Getting Started

To get a local copy up and running follow these steps.
 
### Prerequisites
- [TPC-H](https://www.tpc.org/tpch/) sample data files. Refer [here](https://tedamoh.com/en/blog/55-data-modeling/78-generating-large-example-data-with-tpc-h) for information on using the TPC-H tool dbgen.
- [PostgreSQL](https://www.postgresql.org/)

### Installation 

#### Importing the TPC-H dataset into your PostgreSQL database

1. Navigate to the `/data` directory
```bash
cd data
```
2. Move your TPC-H .tbl files into the `/data` directory
3. If needed, run the `remove_trailing_delimiter.sh` script to remove trailing delimiters
```bash
./remove_trailing_delimiter.sh
```
4. Run the `init.pgsql` file to create tables and import data from the .tbl files into the PostgreSQL database. Make sure to replace `username` with the actual PostgreSQL username you intend to use.
```bash
psql -U username -d TPC-H -a -f init.pgsql
```

## Contributions
This project was built by the following:
1. [Peh Yu Ze](https://github.com/pehyuze)
2. [Lee Ding Zheng](https://github.com/leedz31)
3. [Johnathan Chow](https://github.com/john14759)
4. [Tai Chen An](https://github.com/taica00)
