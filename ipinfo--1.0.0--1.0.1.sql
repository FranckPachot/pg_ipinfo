DROP FUNCTION IF EXISTS getipinfo();

CREATE FUNCTION getipinfo(path text default '/')
 RETURNS TEXT AS 'ipinfo'
 LANGUAGE C IMMUTABLE STRICT;
