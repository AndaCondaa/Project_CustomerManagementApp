CREATE TABLE customer (
	customer_key number(5) NOT NULL,
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

CREATE TABLE product (
	product_key number(4) NOT NULL,
    type_id number(2) NOT NULL,
	product_name varchar2(10) NOT NULL,
	product_price number(6) NOT NULL,
	stock number(5) not null,
	PRIMARY KEY(product_key),
    CONSTRAINT type_fk FOREIGN KEY (type_id)
    REFERENCES product_type(type_id)
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

SELECT *
from product ;

delete from customer;
delete from product;

commit;
