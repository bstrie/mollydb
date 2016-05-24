SELECT size, mdb_size_pretty(size), mdb_size_pretty(-1 * size) FROM
    (VALUES (10::bigint), (1000::bigint), (1000000::bigint),
            (1000000000::bigint), (1000000000000::bigint),
            (1000000000000000::bigint)) x(size);

SELECT size, mdb_size_pretty(size), mdb_size_pretty(-1 * size) FROM
    (VALUES (10::numeric), (1000::numeric), (1000000::numeric),
            (1000000000::numeric), (1000000000000::numeric),
            (1000000000000000::numeric),
            (10.5::numeric), (1000.5::numeric), (1000000.5::numeric),
            (1000000000.5::numeric), (1000000000000.5::numeric),
            (1000000000000000.5::numeric)) x(size);

SELECT size, mdb_size_bytes(size) FROM
    (VALUES ('1'), ('123bytes'), ('1kB'), ('1MB'), (' 1 GB'), ('1.5 GB '),
            ('1TB'), ('3000 TB'), ('1e6 MB')) x(size);

-- case-insensitive units are supported
SELECT size, mdb_size_bytes(size) FROM
    (VALUES ('1'), ('123bYteS'), ('1kb'), ('1mb'), (' 1 Gb'), ('1.5 gB '),
            ('1tb'), ('3000 tb'), ('1e6 mb')) x(size);

-- negative numbers are supported
SELECT size, mdb_size_bytes(size) FROM
    (VALUES ('-1'), ('-123bytes'), ('-1kb'), ('-1mb'), (' -1 Gb'), ('-1.5 gB '),
            ('-1tb'), ('-3000 TB'), ('-10e-1 MB')) x(size);

-- different cases with allowed points
SELECT size, mdb_size_bytes(size) FROM
     (VALUES ('-1.'), ('-1.kb'), ('-1. kb'), ('-0. gb'),
             ('-.1'), ('-.1kb'), ('-.1 kb'), ('-.0 gb')) x(size);

-- invalid inputs
SELECT mdb_size_bytes('1 AB');
SELECT mdb_size_bytes('1 AB A');
SELECT mdb_size_bytes('1 AB A    ');
SELECT mdb_size_bytes('9223372036854775807.9');
SELECT mdb_size_bytes('1e100');
SELECT mdb_size_bytes('1e1000000000000000000');
SELECT mdb_size_bytes('1 byte');  -- the singular "byte" is not supported
SELECT mdb_size_bytes('');

SELECT mdb_size_bytes('kb');
SELECT mdb_size_bytes('..');
SELECT mdb_size_bytes('-.');
SELECT mdb_size_bytes('-.kb');
SELECT mdb_size_bytes('-. kb');

SELECT mdb_size_bytes('.+912');
SELECT mdb_size_bytes('+912+ kB');
SELECT mdb_size_bytes('++123 kB');
