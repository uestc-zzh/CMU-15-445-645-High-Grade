-- ".mode line" is similar to MySQL \G option
-- ".timer on" will output the time cost of this query

-- Please comment out the following lines before submit.
-- .mode line
-- .timer on

SELECT COUNT(DISTINCT(a2.artist))
FROM
    artist_credit_name AS a1,
    artist_credit_name AS a2,
    artist AS a3,
    artist_credit AS a4
WHERE 
    -- JOIN START --
    a1.artist = a3.id AND
    a1.artist_credit = a4.id AND
    a2.artist_credit = a4.id AND
    --- JOIN END ---
    a3.name = "Ariana Grande" AND
    a4.artist_count > 1

;
