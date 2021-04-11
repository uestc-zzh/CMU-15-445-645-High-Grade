-- ".mode line" is similar to MySQL \G option
-- ".timer on" will output the time cost of this query

-- Please comment out the following lines before submit.
-- .mode line
-- .timer on

WITH temp(id1, id2, cnt) AS (
    SELECT
        a1.artist,
        a2.artist,
        COUNT(1)
    FROM
        language AS l,
        artist_credit_name AS a1,
        artist_credit_name AS a2,
        artist AS a3,
        artist AS a4,
        artist_type AS a5,
        artist_type AS a6,
        release AS r
    WHERE
    -------------- JOIN START --------------
        a3.type = a5.id AND
        a4.type = a6.id AND
        r.language = l.id AND
        a1.artist_credit = r.artist_credit AND
        a2.artist_credit = r.artist_credit AND
        a1.artist = a3.id AND
        a2.artist = a4.id AND
        a1.artist_credit = a2.artist_credit AND
    --------------- JOIN END ---------------
        a5.name = "Person" AND
        a6.name = "Person" AND
        l.name = "English" AND
        a3.begin_date_year > 1960 AND
        a4.begin_date_year > 1960 AND
        a1.name < a2.name
    GROUP BY
        a1.artist,
        a2.artist
)

SELECT *
FROM (
    SELECT 
        ROW_NUMBER() OVER (
            ORDER BY 
                cnt DESC,
                a1.name ASC,
                a2.name ASC
        ) AS rank,
        a1.name AS name1,
        a2.name AS name2,
        t.cnt
    FROM 
        temp AS t,
        artist AS a1,
        artist AS a2
    WHERE
    -------------- JOIN START --------------
        t.id1 = a1.id AND
        t.id2 = a2.id
    --------------- JOIN END ---------------
)
WHERE
    name1 = "Dr. Dre" AND
    name2 = "Eminem"

;
