-- ".mode line" is similar to MySQL \G option
-- ".timer on" will output the time cost of this query

-- Please comment out the following lines before submit.
-- .mode line
-- .timer on

SELECT DISTINCT(r1.name)
FROM
    release AS r1,
    release_info AS r2,
    medium AS m,
    medium_format AS m2,
    artist_credit AS a
WHERE
    -- JOIN START --
    r1.artist_credit = a.id AND
    r1.id = m.release AND
    r1.id = r2.release AND
    m.format = m2.id AND
    --- JOIN END ---
    a.name = "Coldplay" AND
    m2.name LIKE '%vinyl%' AND
    m2.name NOT LIKE '%vinyldisc%'
ORDER BY
    r2.date_year ASC,
    r2.date_month ASC,
    r2.date_day ASC

;
