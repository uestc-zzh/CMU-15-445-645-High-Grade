-- ".mode line" is similar to MySQL \G option
-- ".timer on" will output the time cost of this query

-- Please comment out the following lines before submit.
-- .mode line
-- .timer on

WITH 
    max_len_by_type (max_len, work_type_id) AS (
        SELECT 
            MAX(LENGTH(w.name)),
            w.type
        FROM work AS w
        GROUP BY w.type
    )

SELECT 
    w1.name,
    w2.name
FROM
    work AS w1,
    work_type AS w2,
    max_len_by_type AS m
WHERE 
    w1.type = w2.id AND
    w1.type = m.work_type_id AND
    LENGTH(w1.name) = m.max_len
ORDER BY
    w2.id ASC,
    w1.name ASC


;
