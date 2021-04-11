-- ".mode line" is similar to MySQL \G option
-- ".timer on" will output the time cost of this query

-- Please comment out the following lines before submit.
-- .mode line
-- .timer on

WITH
    alias (alias_name) AS (
        SELECT a2.name
        FROM
            artist AS a1,
            artist_alias AS a2
        WHERE
            -- JOIN START --
            a1.id = a2.artist AND
            --- JOIN END ---
            a1.name = "The Beatles"
        ORDER BY
            a2.id
    )

SELECT GROUP_CONCAT(alias_name, ", ")
FROM alias

;