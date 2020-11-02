create database if not exists meconium;
create table if not exists meconium.message (
    id int(10) unsigned not null auto_increment,
    data varchar(255) not null,
    date_created timestamp not null default current_timestamp,
    primary key (id)
) engine = innodb default charset = utf8;
