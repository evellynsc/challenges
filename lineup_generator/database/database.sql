CREATE DATABASE players_db WITH OWNER evellyn;

CREATE TABLE slates
(
    name varchar(120) not null,
    CONSTRAINT pk_slates PRIMARY KEY (name)
);

CREATE TABLE tags
(
    name varchar(30) not null,
    CONSTRAINT pk_tag PRIMARY KEY (name)
);

CREATE TABLE players
(
    fanduel_id int not null,
    first_name varchar(120),
    last_name varchar(120),
    position varchar(10),
    team varchar(10),
    image text,
    CONSTRAINT pk_player PRIMARY KEY (fanduel_id)
);

CREATE TABLE fanduel_projections
(
    id                  int not null REFERENCES players (fanduel_id)  ON DELETE CASCADE,
    score               float8,
    value               float8,
    salary              float8,
    production_rating   int,
    matchup_rating      int,
    fppg                float8,
    ownership           float8,
    opponent_team       varchar(10),
    CONSTRAINT pk_fanduel PRIMARY KEY (id)
);

CREATE TABLE constraints
(
    id                              SERIAL,
    min_salary_sum                  float8,
    max_salary_sum                  float8,
    ownership_sum                   float8,
    min_distinct_teams              int,
    max_players_from_same_team      int,
    num_players                     int,
    num_d                           int,
    num_qb                          int,
    num_rb                          int,
    num_wr                          int,
    num_te                          int,
    num_k                           int,
    CONSTRAINT pk_constraints PRIMARY KEY (id)
);

INSERT INTO constraints (min_salary_sum, max_salary_sum, ownership_sum,
min_distinct_teams, max_players_from_same_team, num_players, num_qb, num_rb, num_wr, num_te, num_k, num_d) VALUES (59000, 60000, 105.68, 3, 4, 9, 1, 2, 3, 1, 1, 1);

CREATE TABLE fanduel_slates
(
    fanduel_id int not null REFERENCES players (fanduel_id) ON DELETE CASCADE,
    slate_id varchar(120) not null REFERENCES slates (name) ON DELETE RESTRICT,
    CONSTRAINT pk_fanduel_slates PRIMARY KEY (fanduel_id, slate_id)
);

CREATE TABLE players_tags
(
    fanduel_id int not null REFERENCES players (fanduel_id) ON DELETE CASCADE,
    tag_id varchar(30) not null REFERENCES tags (name) ON DELETE RESTRICT,
    CONSTRAINT pk_players_tags PRIMARY KEY (fanduel_id, tag_id)
);

CREATE TABLE lineups
(
    id SERIAL not null UNIQUE,
    qb_id           int not null REFERENCES players (fanduel_id) ON DELETE CASCADE,
    rb1_id          int not null REFERENCES players (fanduel_id) ON DELETE CASCADE,
    rb2_id          int not null REFERENCES players (fanduel_id) ON DELETE CASCADE,
    wr1_id          int not null REFERENCES players (fanduel_id) ON DELETE CASCADE,
    wr2_id          int not null REFERENCES players (fanduel_id) ON DELETE CASCADE,
    wr3_id          int not null REFERENCES players (fanduel_id) ON DELETE CASCADE,
    te_id          int not null REFERENCES players (fanduel_id) ON DELETE CASCADE,
    k_id          int not null REFERENCES players (fanduel_id) ON DELETE CASCADE,
    d_id          int not null REFERENCES players (fanduel_id) ON DELETE CASCADE,
    CONSTRAINT pk_lineups PRIMARY KEY (qb_id, rb1_id, rb2_id, wr1_id, wr2_id, wr3_id, te_id, k_id, d_id)
);
