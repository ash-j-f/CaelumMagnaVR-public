--Run once for each child table of gaia_main.
update gaia_main_0 set random_index = floor(random() * (2147483647::bigint - -2147483648::bigint + 1) + -2147483648::bigint)::int; --Done
update gaia_main_1 set random_index = floor(random() * (2147483647::bigint - -2147483648::bigint + 1) + -2147483648::bigint)::int; --Done
update gaia_main_2 set random_index = floor(random() * (2147483647::bigint - -2147483648::bigint + 1) + -2147483648::bigint)::int; --Done
update gaia_main_3 set random_index = floor(random() * (2147483647::bigint - -2147483648::bigint + 1) + -2147483648::bigint)::int; --Done
update gaia_main_4 set random_index = floor(random() * (2147483647::bigint - -2147483648::bigint + 1) + -2147483648::bigint)::int; --Done
update gaia_main_5 set random_index = floor(random() * (2147483647::bigint - -2147483648::bigint + 1) + -2147483648::bigint)::int; --Done
update gaia_main_6 set random_index = floor(random() * (2147483647::bigint - -2147483648::bigint + 1) + -2147483648::bigint)::int; --Done
update gaia_main_7 set random_index = floor(random() * (2147483647::bigint - -2147483648::bigint + 1) + -2147483648::bigint)::int; --Done
update gaia_main_8 set random_index = floor(random() * (2147483647::bigint - -2147483648::bigint + 1) + -2147483648::bigint)::int; --Done
update gaia_main_9 set random_index = floor(random() * (2147483647::bigint - -2147483648::bigint + 1) + -2147483648::bigint)::int; --Done (But was crashing).
update gaia_main_10 set random_index = floor(random() * (2147483647::bigint - -2147483648::bigint + 1) + -2147483648::bigint)::int; --Done
update gaia_main_11 set random_index = floor(random() * (2147483647::bigint - -2147483648::bigint + 1) + -2147483648::bigint)::int; --Done

--HACK to find possible corrupted data in gaia_main_9
create extension dblink;
CREATE OR REPLACE FUNCTION set_random_index_on_gaia_main_9() RETURNS INT AS $$
DECLARE
	iquery text;
	iresult text;
	ids record;
	c int;
BEGIN
	
	--Use DBlink to allow each update to commit as its own transaction, which is impossible inside a Pl/PgSQL function.
	PERFORM dblink_connect('dbname=cm user=postgres password=postgres');
	
	c :=1;
	
	FOR ids IN select source_id from gaia_main_9 where random_index is null order by source_id
	LOOP

		--RAISE NOTICE '% %', ids.source_id, c;

		iquery := format(E'update gaia_main_9 set random_index = floor(random() * (2147483647::bigint - -2147483648::bigint + 1) + -2147483648::bigint)::int where source_id = %L', ids.source_id);
		
		PERFORM dblink_exec(iquery);
		
		--SELECT into iresult dblink_exec(iquery);
		
		--RAISE NOTICE '%', iresult;
		
		c:= c + 1;
		
	END LOOP;
	
	RETURN 0;
END;
$$ LANGUAGE plpgsql;




--OBSOLETE:
create extension dblink;
CREATE OR REPLACE FUNCTION import_gaia_random_index(startval int, endval int) RETURNS INT AS $$
DECLARE
	fpath text;
	iquery text;
	iresult text;
	maxcount int;
	curnumber int;
	rec record;
BEGIN
	
	--Dir name is hard coded for safety. Trailing slash in path name is optional.
	fpath := 'V:\gaia\cdn.gea.esac.esa.int\Gaia\gdr2\gaia_source\csv';
	
	drop table if exists csv_gz_files;
	create temporary table csv_gz_files(filename text);
	
	--Get all csv.gz files.in target dir. 
	execute format(E'copy csv_gz_files from program \'dir /B %I\\*.csv.gz\'', fpath);
	
	maxcount := (endval + 1) - startval;
	
	--Use DBlink to allow each update to commit as its own transaction, which is impossible inside a Pl/PgSQL function.
	PERFORM dblink_connect('dbname=cm user=postgres password=postgres');
	
	curnumber := startval;
	
	--Files are ordered by the first numeric value in the file names, eg: GaiaSource_4295806720_477948256339712.csv.gz is ordered by value 4295806720.
	FOR rec IN select filename from csv_gz_files order by split_part(split_part(filename, '.', 1), '_', 2)::numeric asc offset startval limit maxcount
	LOOP

		RAISE NOTICE 'Processing file number %: %', curnumber, rec.filename;
		
		PERFORM dblink_exec('
		create temporary table t (
			solution_id TEXT,
			designation TEXT,
			source_id bigint,
			random_index bigint,
			ref_epoch TEXT,
			ra numeric,
			ra_error TEXT,
			dec numeric,
			dec_error TEXT,
			parallax TEXT,
			parallax_error numeric,
			parallax_over_error TEXT,
			pmra TEXT,
			pmra_error TEXT,
			pmdec TEXT,
			pmdec_error TEXT,
			ra_dec_corr TEXT,
			ra_parallax_corr TEXT,
			ra_pmra_corr TEXT,
			ra_pmdec_corr TEXT,
			dec_parallax_corr TEXT,
			dec_pmra_corr TEXT,
			dec_pmdec_corr TEXT,
			parallax_pmra_corr TEXT,
			parallax_pmdec_corr TEXT,
			pmra_pmdec_corr TEXT,
			astrometric_n_obs_al TEXT,
			astrometric_n_obs_ac TEXT,
			astrometric_n_good_obs_al TEXT,
			astrometric_n_bad_obs_al TEXT,
			astrometric_gof_al TEXT,
			astrometric_chi2_al TEXT,
			astrometric_excess_noise TEXT,
			astrometric_excess_noise_sig TEXT,
			astrometric_params_solved TEXT,
			astrometric_primary_flag TEXT,
			astrometric_weight_al TEXT,
			astrometric_pseudo_colour TEXT,
			astrometric_pseudo_colour_error TEXT,
			mean_varpi_factor_al TEXT,
			astrometric_matched_observations TEXT,
			visibility_periods_used TEXT,
			astrometric_sigma5d_max TEXT,
			frame_rotator_object_type TEXT,
			matched_observations TEXT,
			duplicated_source TEXT,
			phot_g_n_obs TEXT,
			phot_g_mean_flux TEXT,
			phot_g_mean_flux_error TEXT,
			phot_g_mean_flux_over_error TEXT,
			phot_g_mean_mag TEXT,
			phot_bp_n_obs TEXT,
			phot_bp_mean_flux TEXT,
			phot_bp_mean_flux_error TEXT,
			phot_bp_mean_flux_over_error TEXT,
			phot_bp_mean_mag TEXT,
			phot_rp_n_obs TEXT,
			phot_rp_mean_flux TEXT,
			phot_rp_mean_flux_error TEXT,
			phot_rp_mean_flux_over_error TEXT,
			phot_rp_mean_mag TEXT,
			phot_bp_rp_excess_factor TEXT,
			phot_proc_mode TEXT,
			bp_rp TEXT,
			bp_g TEXT,
			g_rp TEXT,
			radial_velocity TEXT,
			radial_velocity_error TEXT,
			rv_nb_transits TEXT,
			rv_template_teff TEXT,
			rv_template_logg TEXT,
			rv_template_fe_h TEXT,
			phot_variable_flag TEXT,
			l TEXT,
			b TEXT,
			ecl_lon TEXT,
			ecl_lat TEXT,
			priam_flags TEXT,
			teff_val TEXT,
			teff_percentile_lower TEXT,
			teff_percentile_upper TEXT,
			a_g_val TEXT,
			a_g_percentile_lower TEXT,
			a_g_percentile_upper TEXT,
			e_bp_min_rp_val TEXT,
			e_bp_min_rp_percentile_lower TEXT,
			e_bp_min_rp_percentile_upper TEXT,
			flame_flags TEXT,
			radius_val TEXT,
			radius_percentile_lower TEXT,
			radius_percentile_upper TEXT,
			lum_val TEXT,
			lum_percentile_lower TEXT,
			lum_percentile_upper TEXT
		);
		');
		
		iquery := format(E'copy t(solution_id,designation,source_id,random_index,ref_epoch,ra,ra_error,dec,dec_error,parallax,parallax_error,parallax_over_error,pmra,pmra_error,pmdec,pmdec_error,ra_dec_corr,ra_parallax_corr,ra_pmra_corr,ra_pmdec_corr,dec_parallax_corr,dec_pmra_corr,dec_pmdec_corr,parallax_pmra_corr,parallax_pmdec_corr,pmra_pmdec_corr,astrometric_n_obs_al,astrometric_n_obs_ac,astrometric_n_good_obs_al,astrometric_n_bad_obs_al,astrometric_gof_al,astrometric_chi2_al,astrometric_excess_noise,astrometric_excess_noise_sig,astrometric_params_solved,astrometric_primary_flag,astrometric_weight_al,astrometric_pseudo_colour,astrometric_pseudo_colour_error,mean_varpi_factor_al,astrometric_matched_observations,visibility_periods_used,astrometric_sigma5d_max,frame_rotator_object_type,matched_observations,duplicated_source,phot_g_n_obs,phot_g_mean_flux,phot_g_mean_flux_error,phot_g_mean_flux_over_error,phot_g_mean_mag,phot_bp_n_obs,phot_bp_mean_flux,phot_bp_mean_flux_error,phot_bp_mean_flux_over_error,phot_bp_mean_mag,phot_rp_n_obs,phot_rp_mean_flux,phot_rp_mean_flux_error,phot_rp_mean_flux_over_error,phot_rp_mean_mag,phot_bp_rp_excess_factor,phot_proc_mode,bp_rp,bp_g,g_rp,radial_velocity,radial_velocity_error,rv_nb_transits,rv_template_teff,rv_template_logg,rv_template_fe_h,phot_variable_flag,l,b,ecl_lon,ecl_lat,priam_flags,teff_val,teff_percentile_lower,teff_percentile_upper,a_g_val,a_g_percentile_lower,a_g_percentile_upper,e_bp_min_rp_val,e_bp_min_rp_percentile_lower,e_bp_min_rp_percentile_upper,flame_flags,radius_val,radius_percentile_lower,radius_percentile_upper,lum_val,lum_percentile_lower,lum_percentile_upper) from program \'C:\\cygwin64\\bin\\gzip -dcq %I\' DELIMITER \',\' CSV HEADER;', fpath || '\' || rec.filename);
		
		PERFORM dblink_exec(iquery);
		
		SELECT into iresult dblink_exec('update gaia_main set random_index = t.random_index from t where gaia_main.source_id = t.source_id;');
		
		RAISE NOTICE '%', iresult;
		
		PERFORM dblink_exec('drop table t;');
		
		curnumber := curnumber + 1;
		
	END LOOP;
	
	drop table if exists csv_gz_files;
	
	RETURN 0;
END;
$$ LANGUAGE plpgsql;