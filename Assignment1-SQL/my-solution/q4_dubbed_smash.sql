-- ".mode line" is similar to MySQL \G option
-- ".timer on" will output the time cost of this query

-- Please comment out the following lines before submit.
-- .mode line
-- .timer on

SELECT 
    a1.name,
    COUNT(DISTINCT(a3.name)) AS distinct_alias_count
FROM
    artist AS a1,
    area AS a2,
    artist_alias AS a3
WHERE
    -- JOIN START --
    a1.area = 221 AND      -- This is the id of 'United Kingdom', hard coded here to accelerate the query
    -- a1.area = a2.id AND -- Non-hard-coded version
    a1.id = a3.artist AND
    --- JOIN END ---
    a2.name = "United Kingdom" AND
    a1.begin_date_year > 1950
GROUP BY a3.artist
ORDER BY distinct_alias_count DESC
LIMIT 10

;
