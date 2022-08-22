-- ".mode line" is similar to MySQL \G option
-- ".timer on" will output the time cost of this query

-- Please comment out the following lines before submit.
-- .mode line
-- .timer on

SELECT (r2.date_year / 10 * 10) || "s" AS decade, COUNT(1) AS release_count
FROM
    release AS r1,
    release_info AS r2,
    release_status AS r3
WHERE
    -- JOIN START --
    r1.status = r3.id AND
    r1.id = r2.release AND
    --- JOIN END ---
    r3.name = "Official" AND
    r2.date_year >= 1900
GROUP BY decade
ORDER BY
    release_count DESC,
    decade DESC


;
