CREATE TABLE gaia_distances(
	source_id bigint primary key,
	r_est double precision --Gaia distances catalogue uses double precision floats with up to 15 decimal places.
);

CREATE LANGUAGE plpgsql; --Will fail safely if plpgsql is already installed

CREATE OR REPLACE FUNCTION import_distest(startval int, endval int, padcount int) RETURNS INT AS $$
DECLARE
	curval int;
	filenumber text;
	filename text;
BEGIN
	
	curval := startval;
	
	WHILE curval != endval + 1 LOOP
	
		filenumber := LPAD((curval)::text, padcount, '0');

		filename := 'V:\gaia\gdr2_distances\distest_q' || filenumber || '.csv';

		RAISE NOTICE 'Processing file %', filename;

		create temporary table t (source_id bigint, r_est double precision, r_lo double precision, r_hi double precision, r_len double precision, result_flag smallint, modality_flag smallint);
		
		EXECUTE format(E'copy t(source_id,r_est,r_lo,r_hi,r_len,result_flag,modality_flag) from %L DELIMITER \',\' CSV HEADER', filename);
		
		--Skip invalid distance estimates (result_flag is 0). This occurs for a very small number of stars (3278 in total).
		--The reasons for invalid estimates are explained in the paper ESTIMATING DISTANCES FROM PARALLAXES IV: DISTANCES TO 1.33 BILLION STARS IN GAUA DATA RELEASE 2 https://arxiv.org/pdf/1804.10121.pdf by Bailer-Jones et al.
		insert into gaia_distances (source_id, r_est) select source_id, r_est from t where result_flag != 0 ON CONFLICT DO NOTHING;
		
		drop table if exists t;
		
		curval := curval + 1;
		
	END LOOP;
	
	RETURN 0;
END;
$$ LANGUAGE plpgsql;