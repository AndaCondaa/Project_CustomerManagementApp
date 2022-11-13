CREATE TABLE customer_table (
	customer_key number(10) NOT NULL,
	clinic_name varchar2(20) NOT NULL,
	license_number varchar2(10) NOT NULL,
	dentist_name varchar2(20) NOT NULL,
	phone_number varchar2(15),
    order_amount number(10),
	PRIMARY KEY(customer_key)
);

CREATE TABLE product_type (
    type_id number(2) not null,
    type_name varchar2(10),
    PRIMARY KEY(type_id)
);

CREATE TABLE product_table (
	product_key number(10) NOT NULL,
    type_id number(2) NOT NULL,
	product_name varchar2(10) NOT NULL,
	product_price number(6) NOT NULL,
	stock number(5) not null,
	PRIMARY KEY(product_key),
    CONSTRAINT type_fk FOREIGN KEY (type_id)
    REFERENCES product_type(type_id)
);

CREATE TABLE order_table (
	order_number varchar2(30) NOT NULL,
    order_ck number(10) NOT NULL,
	order_pk number(10) NOT NULL,
	order_date varchar2(15) NOT NULL,
	quantity number(5) not null,
    total   number(30) not null,
	PRIMARY KEY(order_number),
    CONSTRAINT fk_ck FOREIGN KEY (order_ck)
    REFERENCES customer_table(customer_key),
    CONSTRAINT fk_pk FOREIGN KEY (order_pk)
    REFERENCES product_table(product_key)  
);


CREATE SEQUENCE seq_type_id
INCREMENT BY 1
START WITH 1 ;

INSERT INTO product_type
VALUES (seq_type_id.NEXTVAL, 'IMPLANT');

INSERT INTO product_type
VALUES (seq_type_id.NEXTVAL, 'SCANNER');

INSERT INTO product_type
VALUES (seq_type_id.NEXTVAL, 'CHAIR');

INSERT INTO product_type
VALUES (seq_type_id.NEXTVAL, 'ETC');

SELECT *
from product_type order by type_id;


--drop table order_table;
--drop table customer_table;
--drop table product_table;
--drop table product_type;
--drop SEQUENCE seq_type_id;

commit;


GRANT SELECT ON customer_table TO customer_manager;
commit;


