
CREATE OR REPLACE FUNCTION getipinfo() 
 RETURNS TEXT AS 'ipinfo' 
 LANGUAGE C IMMUTABLE STRICT;
