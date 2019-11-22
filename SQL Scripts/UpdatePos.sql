CREATE LANGUAGE plpgsql; --Will fail safely if plpgsql is already installed

create extension dblink;

CREATE OR REPLACE FUNCTION update_pos(chunksize int) RETURNS INT AS $$
DECLARE
	curchunk int;
	iresult text;
BEGIN
	
	curchunk := 1;
	
	--Use DBlink to allow each update to commit as its own transaction, which is impossible inside a Pl/PgSQL function.
	PERFORM dblink_connect('dbname=cm user=postgres password=postgres');
	
	LOOP
	
		RAISE NOTICE 'Processing chunk %', curchunk;
	
		--Y+ is forward, X+ is right, Z+ is up. Dec+ is up the sky sphere (CCW rotation in X), RA+ is left around sky sphere (CCW rotation in Z), from the 
		--point of view of an observer in the middle of the sphere looking outwards.
		SELECT into iresult dblink_exec('with s as (select source_id from gaia_main where geom is null limit ' || chunksize || ') update gaia_main set geom = ST_Scale(ST_RotateZ( ST_RotateX(''POINT(0 1 0)'', radians(dec)),  radians(ra)), r_est, r_est, r_est) where source_id in (select source_id from s);');
		
		RAISE NOTICE '%', iresult;
		
		--End the update process when there is nothing left to update.
		EXIT when iresult = 'UPDATE 0';
		
		curchunk := curchunk + 1;
		
	END LOOP;
	
	RETURN 0;
END;
$$ LANGUAGE plpgsql;