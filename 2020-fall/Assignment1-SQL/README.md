# Assignment 1 - SQL
[HOMEWORK #1 - SQL](https://15445.courses.cs.cmu.edu/fall2020/homework1/)

Go to the release page and download **musicbrainz-cmudb2020.db.gz** file.  
Unzip it and you can execute sql query in that database by:  
`sqlite3 musicbrainz-cmudb2020.db < [your-sql-script]`

# Content
| File/Folder                           | Description                           |
| :------------------------------------ | :------------------------------------ |
| my-output                             | Output produced by my solutions       |
| my-solution                           | My solutions                          |
| solution                              | Official solutions                    |
| solution-output                       | Output produced by official solutions |
| README.md                             | The file you are reading now          |


# SQLite Notes
1. **View table schema**  
`.schema [table name]`
 
2. **Clear screen**  
`.shell clear`

3. **Quit**  
`.quit`

4. **Run SQL script**  
`sqlite3 [db file] < [your-sql-script]`

5. **Print column name in query output**  
`.header on`  
`.mode column`

6. **Equivalent to MySQL \G output format**  
`.mode line`

7. **Get time cost**  
`.timer on`