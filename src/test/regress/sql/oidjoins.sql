--
-- This is created by mdb/src/tools/findoidjoins/make_oidjoins_check
--
SELECT	ctid, aggfnoid
FROM	mdb_catalog.mdb_aggregate fk
WHERE	aggfnoid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.aggfnoid);
SELECT	ctid, aggtransfn
FROM	mdb_catalog.mdb_aggregate fk
WHERE	aggtransfn != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.aggtransfn);
SELECT	ctid, aggfinalfn
FROM	mdb_catalog.mdb_aggregate fk
WHERE	aggfinalfn != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.aggfinalfn);
SELECT	ctid, aggmtransfn
FROM	mdb_catalog.mdb_aggregate fk
WHERE	aggmtransfn != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.aggmtransfn);
SELECT	ctid, aggminvtransfn
FROM	mdb_catalog.mdb_aggregate fk
WHERE	aggminvtransfn != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.aggminvtransfn);
SELECT	ctid, aggmfinalfn
FROM	mdb_catalog.mdb_aggregate fk
WHERE	aggmfinalfn != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.aggmfinalfn);
SELECT	ctid, aggsortop
FROM	mdb_catalog.mdb_aggregate fk
WHERE	aggsortop != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_operator pk WHERE pk.oid = fk.aggsortop);
SELECT	ctid, aggtranstype
FROM	mdb_catalog.mdb_aggregate fk
WHERE	aggtranstype != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.aggtranstype);
SELECT	ctid, aggmtranstype
FROM	mdb_catalog.mdb_aggregate fk
WHERE	aggmtranstype != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.aggmtranstype);
SELECT	ctid, amhandler
FROM	mdb_catalog.mdb_am fk
WHERE	amhandler != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.amhandler);
SELECT	ctid, amopfamily
FROM	mdb_catalog.mdb_amop fk
WHERE	amopfamily != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_opfamily pk WHERE pk.oid = fk.amopfamily);
SELECT	ctid, amoplefttype
FROM	mdb_catalog.mdb_amop fk
WHERE	amoplefttype != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.amoplefttype);
SELECT	ctid, amoprighttype
FROM	mdb_catalog.mdb_amop fk
WHERE	amoprighttype != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.amoprighttype);
SELECT	ctid, amopopr
FROM	mdb_catalog.mdb_amop fk
WHERE	amopopr != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_operator pk WHERE pk.oid = fk.amopopr);
SELECT	ctid, amopmethod
FROM	mdb_catalog.mdb_amop fk
WHERE	amopmethod != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_am pk WHERE pk.oid = fk.amopmethod);
SELECT	ctid, amopsortfamily
FROM	mdb_catalog.mdb_amop fk
WHERE	amopsortfamily != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_opfamily pk WHERE pk.oid = fk.amopsortfamily);
SELECT	ctid, amprocfamily
FROM	mdb_catalog.mdb_amproc fk
WHERE	amprocfamily != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_opfamily pk WHERE pk.oid = fk.amprocfamily);
SELECT	ctid, amproclefttype
FROM	mdb_catalog.mdb_amproc fk
WHERE	amproclefttype != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.amproclefttype);
SELECT	ctid, amprocrighttype
FROM	mdb_catalog.mdb_amproc fk
WHERE	amprocrighttype != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.amprocrighttype);
SELECT	ctid, amproc
FROM	mdb_catalog.mdb_amproc fk
WHERE	amproc != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.amproc);
SELECT	ctid, adrelid
FROM	mdb_catalog.mdb_attrdef fk
WHERE	adrelid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.adrelid);
SELECT	ctid, attrelid
FROM	mdb_catalog.mdb_attribute fk
WHERE	attrelid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.attrelid);
SELECT	ctid, atttypid
FROM	mdb_catalog.mdb_attribute fk
WHERE	atttypid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.atttypid);
SELECT	ctid, attcollation
FROM	mdb_catalog.mdb_attribute fk
WHERE	attcollation != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_collation pk WHERE pk.oid = fk.attcollation);
SELECT	ctid, castsource
FROM	mdb_catalog.mdb_cast fk
WHERE	castsource != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.castsource);
SELECT	ctid, casttarget
FROM	mdb_catalog.mdb_cast fk
WHERE	casttarget != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.casttarget);
SELECT	ctid, castfunc
FROM	mdb_catalog.mdb_cast fk
WHERE	castfunc != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.castfunc);
SELECT	ctid, relnamespace
FROM	mdb_catalog.mdb_class fk
WHERE	relnamespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_namespace pk WHERE pk.oid = fk.relnamespace);
SELECT	ctid, reltype
FROM	mdb_catalog.mdb_class fk
WHERE	reltype != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.reltype);
SELECT	ctid, reloftype
FROM	mdb_catalog.mdb_class fk
WHERE	reloftype != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.reloftype);
SELECT	ctid, relowner
FROM	mdb_catalog.mdb_class fk
WHERE	relowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.relowner);
SELECT	ctid, relam
FROM	mdb_catalog.mdb_class fk
WHERE	relam != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_am pk WHERE pk.oid = fk.relam);
SELECT	ctid, reltablespace
FROM	mdb_catalog.mdb_class fk
WHERE	reltablespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_tablespace pk WHERE pk.oid = fk.reltablespace);
SELECT	ctid, reltoastrelid
FROM	mdb_catalog.mdb_class fk
WHERE	reltoastrelid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.reltoastrelid);
SELECT	ctid, collnamespace
FROM	mdb_catalog.mdb_collation fk
WHERE	collnamespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_namespace pk WHERE pk.oid = fk.collnamespace);
SELECT	ctid, collowner
FROM	mdb_catalog.mdb_collation fk
WHERE	collowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.collowner);
SELECT	ctid, connamespace
FROM	mdb_catalog.mdb_constraint fk
WHERE	connamespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_namespace pk WHERE pk.oid = fk.connamespace);
SELECT	ctid, conrelid
FROM	mdb_catalog.mdb_constraint fk
WHERE	conrelid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.conrelid);
SELECT	ctid, contypid
FROM	mdb_catalog.mdb_constraint fk
WHERE	contypid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.contypid);
SELECT	ctid, conindid
FROM	mdb_catalog.mdb_constraint fk
WHERE	conindid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.conindid);
SELECT	ctid, confrelid
FROM	mdb_catalog.mdb_constraint fk
WHERE	confrelid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.confrelid);
SELECT	ctid, connamespace
FROM	mdb_catalog.mdb_conversion fk
WHERE	connamespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_namespace pk WHERE pk.oid = fk.connamespace);
SELECT	ctid, conowner
FROM	mdb_catalog.mdb_conversion fk
WHERE	conowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.conowner);
SELECT	ctid, conproc
FROM	mdb_catalog.mdb_conversion fk
WHERE	conproc != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.conproc);
SELECT	ctid, datdba
FROM	mdb_catalog.mdb_database fk
WHERE	datdba != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.datdba);
SELECT	ctid, dattablespace
FROM	mdb_catalog.mdb_database fk
WHERE	dattablespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_tablespace pk WHERE pk.oid = fk.dattablespace);
SELECT	ctid, setdatabase
FROM	mdb_catalog.mdb_db_role_setting fk
WHERE	setdatabase != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_database pk WHERE pk.oid = fk.setdatabase);
SELECT	ctid, classid
FROM	mdb_catalog.mdb_depend fk
WHERE	classid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.classid);
SELECT	ctid, refclassid
FROM	mdb_catalog.mdb_depend fk
WHERE	refclassid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.refclassid);
SELECT	ctid, classoid
FROM	mdb_catalog.mdb_description fk
WHERE	classoid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.classoid);
SELECT	ctid, enumtypid
FROM	mdb_catalog.mdb_enum fk
WHERE	enumtypid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.enumtypid);
SELECT	ctid, extowner
FROM	mdb_catalog.mdb_extension fk
WHERE	extowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.extowner);
SELECT	ctid, extnamespace
FROM	mdb_catalog.mdb_extension fk
WHERE	extnamespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_namespace pk WHERE pk.oid = fk.extnamespace);
SELECT	ctid, fdwowner
FROM	mdb_catalog.mdb_foreign_data_wrapper fk
WHERE	fdwowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.fdwowner);
SELECT	ctid, srvowner
FROM	mdb_catalog.mdb_foreign_server fk
WHERE	srvowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.srvowner);
SELECT	ctid, srvfdw
FROM	mdb_catalog.mdb_foreign_server fk
WHERE	srvfdw != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_foreign_data_wrapper pk WHERE pk.oid = fk.srvfdw);
SELECT	ctid, indexrelid
FROM	mdb_catalog.mdb_index fk
WHERE	indexrelid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.indexrelid);
SELECT	ctid, indrelid
FROM	mdb_catalog.mdb_index fk
WHERE	indrelid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.indrelid);
SELECT	ctid, inhrelid
FROM	mdb_catalog.mdb_inherits fk
WHERE	inhrelid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.inhrelid);
SELECT	ctid, inhparent
FROM	mdb_catalog.mdb_inherits fk
WHERE	inhparent != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.inhparent);
SELECT	ctid, lanowner
FROM	mdb_catalog.mdb_language fk
WHERE	lanowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.lanowner);
SELECT	ctid, lanplcallfoid
FROM	mdb_catalog.mdb_language fk
WHERE	lanplcallfoid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.lanplcallfoid);
SELECT	ctid, laninline
FROM	mdb_catalog.mdb_language fk
WHERE	laninline != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.laninline);
SELECT	ctid, lanvalidator
FROM	mdb_catalog.mdb_language fk
WHERE	lanvalidator != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.lanvalidator);
SELECT	ctid, loid
FROM	mdb_catalog.mdb_largeobject fk
WHERE	loid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_largeobject_metadata pk WHERE pk.oid = fk.loid);
SELECT	ctid, lomowner
FROM	mdb_catalog.mdb_largeobject_metadata fk
WHERE	lomowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.lomowner);
SELECT	ctid, nspowner
FROM	mdb_catalog.mdb_namespace fk
WHERE	nspowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.nspowner);
SELECT	ctid, opcmethod
FROM	mdb_catalog.mdb_opclass fk
WHERE	opcmethod != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_am pk WHERE pk.oid = fk.opcmethod);
SELECT	ctid, opcnamespace
FROM	mdb_catalog.mdb_opclass fk
WHERE	opcnamespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_namespace pk WHERE pk.oid = fk.opcnamespace);
SELECT	ctid, opcowner
FROM	mdb_catalog.mdb_opclass fk
WHERE	opcowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.opcowner);
SELECT	ctid, opcfamily
FROM	mdb_catalog.mdb_opclass fk
WHERE	opcfamily != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_opfamily pk WHERE pk.oid = fk.opcfamily);
SELECT	ctid, opcintype
FROM	mdb_catalog.mdb_opclass fk
WHERE	opcintype != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.opcintype);
SELECT	ctid, opckeytype
FROM	mdb_catalog.mdb_opclass fk
WHERE	opckeytype != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.opckeytype);
SELECT	ctid, oprnamespace
FROM	mdb_catalog.mdb_operator fk
WHERE	oprnamespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_namespace pk WHERE pk.oid = fk.oprnamespace);
SELECT	ctid, oprowner
FROM	mdb_catalog.mdb_operator fk
WHERE	oprowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.oprowner);
SELECT	ctid, oprleft
FROM	mdb_catalog.mdb_operator fk
WHERE	oprleft != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.oprleft);
SELECT	ctid, oprright
FROM	mdb_catalog.mdb_operator fk
WHERE	oprright != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.oprright);
SELECT	ctid, oprresult
FROM	mdb_catalog.mdb_operator fk
WHERE	oprresult != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.oprresult);
SELECT	ctid, oprcom
FROM	mdb_catalog.mdb_operator fk
WHERE	oprcom != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_operator pk WHERE pk.oid = fk.oprcom);
SELECT	ctid, oprnegate
FROM	mdb_catalog.mdb_operator fk
WHERE	oprnegate != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_operator pk WHERE pk.oid = fk.oprnegate);
SELECT	ctid, oprcode
FROM	mdb_catalog.mdb_operator fk
WHERE	oprcode != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.oprcode);
SELECT	ctid, oprrest
FROM	mdb_catalog.mdb_operator fk
WHERE	oprrest != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.oprrest);
SELECT	ctid, oprjoin
FROM	mdb_catalog.mdb_operator fk
WHERE	oprjoin != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.oprjoin);
SELECT	ctid, opfmethod
FROM	mdb_catalog.mdb_opfamily fk
WHERE	opfmethod != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_am pk WHERE pk.oid = fk.opfmethod);
SELECT	ctid, opfnamespace
FROM	mdb_catalog.mdb_opfamily fk
WHERE	opfnamespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_namespace pk WHERE pk.oid = fk.opfnamespace);
SELECT	ctid, opfowner
FROM	mdb_catalog.mdb_opfamily fk
WHERE	opfowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.opfowner);
SELECT	ctid, polrelid
FROM	mdb_catalog.mdb_policy fk
WHERE	polrelid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.polrelid);
SELECT	ctid, pronamespace
FROM	mdb_catalog.mdb_proc fk
WHERE	pronamespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_namespace pk WHERE pk.oid = fk.pronamespace);
SELECT	ctid, proowner
FROM	mdb_catalog.mdb_proc fk
WHERE	proowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.proowner);
SELECT	ctid, prolang
FROM	mdb_catalog.mdb_proc fk
WHERE	prolang != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_language pk WHERE pk.oid = fk.prolang);
SELECT	ctid, provariadic
FROM	mdb_catalog.mdb_proc fk
WHERE	provariadic != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.provariadic);
SELECT	ctid, protransform
FROM	mdb_catalog.mdb_proc fk
WHERE	protransform != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.protransform);
SELECT	ctid, prorettype
FROM	mdb_catalog.mdb_proc fk
WHERE	prorettype != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.prorettype);
SELECT	ctid, rngtypid
FROM	mdb_catalog.mdb_range fk
WHERE	rngtypid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.rngtypid);
SELECT	ctid, rngsubtype
FROM	mdb_catalog.mdb_range fk
WHERE	rngsubtype != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.rngsubtype);
SELECT	ctid, rngcollation
FROM	mdb_catalog.mdb_range fk
WHERE	rngcollation != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_collation pk WHERE pk.oid = fk.rngcollation);
SELECT	ctid, rngsubopc
FROM	mdb_catalog.mdb_range fk
WHERE	rngsubopc != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_opclass pk WHERE pk.oid = fk.rngsubopc);
SELECT	ctid, rngcanonical
FROM	mdb_catalog.mdb_range fk
WHERE	rngcanonical != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.rngcanonical);
SELECT	ctid, rngsubdiff
FROM	mdb_catalog.mdb_range fk
WHERE	rngsubdiff != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.rngsubdiff);
SELECT	ctid, ev_class
FROM	mdb_catalog.mdb_rewrite fk
WHERE	ev_class != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.ev_class);
SELECT	ctid, refclassid
FROM	mdb_catalog.mdb_shdepend fk
WHERE	refclassid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.refclassid);
SELECT	ctid, classoid
FROM	mdb_catalog.mdb_shdescription fk
WHERE	classoid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.classoid);
SELECT	ctid, starelid
FROM	mdb_catalog.mdb_statistic fk
WHERE	starelid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.starelid);
SELECT	ctid, staop1
FROM	mdb_catalog.mdb_statistic fk
WHERE	staop1 != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_operator pk WHERE pk.oid = fk.staop1);
SELECT	ctid, staop2
FROM	mdb_catalog.mdb_statistic fk
WHERE	staop2 != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_operator pk WHERE pk.oid = fk.staop2);
SELECT	ctid, staop3
FROM	mdb_catalog.mdb_statistic fk
WHERE	staop3 != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_operator pk WHERE pk.oid = fk.staop3);
SELECT	ctid, staop4
FROM	mdb_catalog.mdb_statistic fk
WHERE	staop4 != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_operator pk WHERE pk.oid = fk.staop4);
SELECT	ctid, staop5
FROM	mdb_catalog.mdb_statistic fk
WHERE	staop5 != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_operator pk WHERE pk.oid = fk.staop5);
SELECT	ctid, spcowner
FROM	mdb_catalog.mdb_tablespace fk
WHERE	spcowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.spcowner);
SELECT	ctid, trftype
FROM	mdb_catalog.mdb_transform fk
WHERE	trftype != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.trftype);
SELECT	ctid, trflang
FROM	mdb_catalog.mdb_transform fk
WHERE	trflang != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_language pk WHERE pk.oid = fk.trflang);
SELECT	ctid, trffromsql
FROM	mdb_catalog.mdb_transform fk
WHERE	trffromsql != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.trffromsql);
SELECT	ctid, trftosql
FROM	mdb_catalog.mdb_transform fk
WHERE	trftosql != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.trftosql);
SELECT	ctid, tgrelid
FROM	mdb_catalog.mdb_trigger fk
WHERE	tgrelid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.tgrelid);
SELECT	ctid, tgfoid
FROM	mdb_catalog.mdb_trigger fk
WHERE	tgfoid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.tgfoid);
SELECT	ctid, tgconstrrelid
FROM	mdb_catalog.mdb_trigger fk
WHERE	tgconstrrelid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.tgconstrrelid);
SELECT	ctid, tgconstrindid
FROM	mdb_catalog.mdb_trigger fk
WHERE	tgconstrindid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.tgconstrindid);
SELECT	ctid, tgconstraint
FROM	mdb_catalog.mdb_trigger fk
WHERE	tgconstraint != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_constraint pk WHERE pk.oid = fk.tgconstraint);
SELECT	ctid, cfgnamespace
FROM	mdb_catalog.mdb_ts_config fk
WHERE	cfgnamespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_namespace pk WHERE pk.oid = fk.cfgnamespace);
SELECT	ctid, cfgowner
FROM	mdb_catalog.mdb_ts_config fk
WHERE	cfgowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.cfgowner);
SELECT	ctid, cfgparser
FROM	mdb_catalog.mdb_ts_config fk
WHERE	cfgparser != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_ts_parser pk WHERE pk.oid = fk.cfgparser);
SELECT	ctid, mapcfg
FROM	mdb_catalog.mdb_ts_config_map fk
WHERE	mapcfg != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_ts_config pk WHERE pk.oid = fk.mapcfg);
SELECT	ctid, mapdict
FROM	mdb_catalog.mdb_ts_config_map fk
WHERE	mapdict != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_ts_dict pk WHERE pk.oid = fk.mapdict);
SELECT	ctid, dictnamespace
FROM	mdb_catalog.mdb_ts_dict fk
WHERE	dictnamespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_namespace pk WHERE pk.oid = fk.dictnamespace);
SELECT	ctid, dictowner
FROM	mdb_catalog.mdb_ts_dict fk
WHERE	dictowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.dictowner);
SELECT	ctid, dicttemplate
FROM	mdb_catalog.mdb_ts_dict fk
WHERE	dicttemplate != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_ts_template pk WHERE pk.oid = fk.dicttemplate);
SELECT	ctid, prsnamespace
FROM	mdb_catalog.mdb_ts_parser fk
WHERE	prsnamespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_namespace pk WHERE pk.oid = fk.prsnamespace);
SELECT	ctid, prsstart
FROM	mdb_catalog.mdb_ts_parser fk
WHERE	prsstart != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.prsstart);
SELECT	ctid, prstoken
FROM	mdb_catalog.mdb_ts_parser fk
WHERE	prstoken != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.prstoken);
SELECT	ctid, prsend
FROM	mdb_catalog.mdb_ts_parser fk
WHERE	prsend != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.prsend);
SELECT	ctid, prsheadline
FROM	mdb_catalog.mdb_ts_parser fk
WHERE	prsheadline != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.prsheadline);
SELECT	ctid, prslextype
FROM	mdb_catalog.mdb_ts_parser fk
WHERE	prslextype != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.prslextype);
SELECT	ctid, tmplnamespace
FROM	mdb_catalog.mdb_ts_template fk
WHERE	tmplnamespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_namespace pk WHERE pk.oid = fk.tmplnamespace);
SELECT	ctid, tmplinit
FROM	mdb_catalog.mdb_ts_template fk
WHERE	tmplinit != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.tmplinit);
SELECT	ctid, tmpllexize
FROM	mdb_catalog.mdb_ts_template fk
WHERE	tmpllexize != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.tmpllexize);
SELECT	ctid, typnamespace
FROM	mdb_catalog.mdb_type fk
WHERE	typnamespace != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_namespace pk WHERE pk.oid = fk.typnamespace);
SELECT	ctid, typowner
FROM	mdb_catalog.mdb_type fk
WHERE	typowner != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_authid pk WHERE pk.oid = fk.typowner);
SELECT	ctid, typrelid
FROM	mdb_catalog.mdb_type fk
WHERE	typrelid != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_class pk WHERE pk.oid = fk.typrelid);
SELECT	ctid, typelem
FROM	mdb_catalog.mdb_type fk
WHERE	typelem != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.typelem);
SELECT	ctid, typarray
FROM	mdb_catalog.mdb_type fk
WHERE	typarray != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.typarray);
SELECT	ctid, typinput
FROM	mdb_catalog.mdb_type fk
WHERE	typinput != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.typinput);
SELECT	ctid, typoutput
FROM	mdb_catalog.mdb_type fk
WHERE	typoutput != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.typoutput);
SELECT	ctid, typreceive
FROM	mdb_catalog.mdb_type fk
WHERE	typreceive != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.typreceive);
SELECT	ctid, typsend
FROM	mdb_catalog.mdb_type fk
WHERE	typsend != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.typsend);
SELECT	ctid, typmodin
FROM	mdb_catalog.mdb_type fk
WHERE	typmodin != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.typmodin);
SELECT	ctid, typmodout
FROM	mdb_catalog.mdb_type fk
WHERE	typmodout != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.typmodout);
SELECT	ctid, typanalyze
FROM	mdb_catalog.mdb_type fk
WHERE	typanalyze != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_proc pk WHERE pk.oid = fk.typanalyze);
SELECT	ctid, typbasetype
FROM	mdb_catalog.mdb_type fk
WHERE	typbasetype != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.typbasetype);
SELECT	ctid, typcollation
FROM	mdb_catalog.mdb_type fk
WHERE	typcollation != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_collation pk WHERE pk.oid = fk.typcollation);
SELECT	ctid, conpfeqop
FROM	(SELECT ctid, unnest(conpfeqop) AS conpfeqop FROM mdb_catalog.mdb_constraint) fk
WHERE	conpfeqop != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_operator pk WHERE pk.oid = fk.conpfeqop);
SELECT	ctid, conppeqop
FROM	(SELECT ctid, unnest(conppeqop) AS conppeqop FROM mdb_catalog.mdb_constraint) fk
WHERE	conppeqop != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_operator pk WHERE pk.oid = fk.conppeqop);
SELECT	ctid, conffeqop
FROM	(SELECT ctid, unnest(conffeqop) AS conffeqop FROM mdb_catalog.mdb_constraint) fk
WHERE	conffeqop != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_operator pk WHERE pk.oid = fk.conffeqop);
SELECT	ctid, conexclop
FROM	(SELECT ctid, unnest(conexclop) AS conexclop FROM mdb_catalog.mdb_constraint) fk
WHERE	conexclop != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_operator pk WHERE pk.oid = fk.conexclop);
SELECT	ctid, proallargtypes
FROM	(SELECT ctid, unnest(proallargtypes) AS proallargtypes FROM mdb_catalog.mdb_proc) fk
WHERE	proallargtypes != 0 AND
	NOT EXISTS(SELECT 1 FROM mdb_catalog.mdb_type pk WHERE pk.oid = fk.proallargtypes);
