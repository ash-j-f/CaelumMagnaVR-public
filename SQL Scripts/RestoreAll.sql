--copy gaia_main (source_id, random_index, geom) from program 'C:\cygwin64\bin\gzip -dcq V:\gaia\gaia_main_randindex_geom_only.csv.gz' WITH (FORMAT CSV, DELIMITER ',', HEADER);

copy gaia_main (source_id, ra, dec, r_est, geom) from program 'C:\cygwin64\bin\gzip -dcq V:\gaia\gaia_main_with_3Dgeom.csv.gz' WITH (FORMAT CSV, DELIMITER ',', HEADER);