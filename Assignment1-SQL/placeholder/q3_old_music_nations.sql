-- ".mode line" is similar to MySQL \G option
-- ".timer on" will output the time cost of this query

-- Please comment out the following lines before submit.
-- .mode line
-- .timer on

SELECT a2.name, COUNT(1) AS artist_count
FROM 
    artist AS a1,
    area AS a2
WHERE 
    a1.begin_date_year < 1850 AND
    a1.area = a2.id
GROUP BY a1.area
ORDER BY artist_count DESC
LIMIT 10
;
