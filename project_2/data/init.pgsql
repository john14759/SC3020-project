--region
CREATE TABLE IF NOT EXISTS "region"
(
	r_regionkey integer NOT NULL, 
	r_name character (25) COLLATE pg_catalog."default" NOT NULL, 
	r_comment character varying(152) COLLATE pg_catalog."default", 
	CONSTRAINT region_pkey PRIMARY KEY (r_regionkey) 
) 
WITH ( 
	OIDS = FALSE
); 

--nation
CREATE TABLE IF NOT EXISTS "nation" (
	n_nationkey integer NOT NULL,
	n_name character(25) COLLATE pg_catalog."default" NOT NULL,
	n_regionkey integer NOT NULL,
	n_comment character varying(152) COLLATE pg_catalog."default",
	CONSTRAINT nation_pkey PRIMARY KEY (n_nationkey),
	CONSTRAINT fk_nation FOREIGN KEY (n_regionkey)
		REFERENCES public.region (r_regionkey) MATCH SIMPLE
		ON UPDATE NO ACTION
		ON DELETE NO ACTION 
)
WITH (
	OIDS = FALSE
);

\copy "region" from 'region.tbl' DELIMITER '|' CSV;
\copy "nation" from 'nation.tbl' DELIMITER '|' CSV;
