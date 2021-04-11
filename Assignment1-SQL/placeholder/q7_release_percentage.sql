-- ".mode line" is similar to MySQL \G option
-- ".timer on" will output the time cost of this query

-- Please comment out the following lines before submit.
-- .mode line
-- .timer on


WITH
    count_by_month (date_year, date_month, cnt) AS (
        SELECT
            date_year,
            date_month,
            COUNT(1)
        FROM release_info
        WHERE 201907 <= date_year * 100 + date_month AND date_year * 100 + date_month <= 202007
        GROUP BY
            date_year,
            date_month
    ),
    
    total_count (cnt) AS (
        SELECT COUNT(1)
        FROM release_info
        WHERE 201907 <= date_year * 100 + date_month AND date_year * 100 + date_month <= 202007
    )

SELECT 
    c.date_year || "." || SUBSTR("0" || c.date_month, -2) || "|" || ROUND(100.0 * c.cnt / t.cnt, 2)
FROM 
    count_by_month AS c,
    total_count AS t
ORDER BY
    c.date_year ASC,
    c.date_month ASC

;
