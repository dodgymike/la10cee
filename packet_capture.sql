create table connection_pair (
	id serial not null primary key
	,ipA text not null
	,portA int not null
	,ipB text not null
	,portB int not null
	,start_date timestamp not null
	,end_date timestamp not null
	,state text not null
	,data_in int not null default 0
	,data_out int not null default 0
	,packets_in int not null default 0
	,packets_out int not null default 0
);

