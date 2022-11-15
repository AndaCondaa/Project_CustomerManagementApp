--PROCEDURE

-INPUT_CUSTOMER
create or replace PROCEDURE INPUT_CUSTOMER 
(
    ck       IN CUSTOMER_TABLE.CUSTOMER_KEY%TYPE,
    clinic   IN CUSTOMER_TABLE.CLINIC_NAME%TYPE,
    license  IN CUSTOMER_TABLE.LICENSE_NUMBER%TYPE,
    dentist  IN CUSTOMER_TABLE.DENTIST_NAME%TYPE,
    number_c IN CUSTOMER_TABLE.PHONE_NUMBER%TYPE,
    order_count   IN CUSTOMER_TABLE.ORDER_COUNT%TYPE 
)
IS
BEGIN
    INSERT INTO CUSTOMER_TABLE(CUSTOMER_KEY, CLINIC_NAME, LICENSE_NUMBER, DENTIST_NAME, PHONE_NUMBER, ORDER_COUNT)
    VALUES  (ck, clinic, license, dentist, number_c, order_count);
END INPUT_CUSTOMER;


--INPUT_PRODUCT
create or replace PROCEDURE INPUT_PRODUCT
(
    pk       IN product_table.product_key%TYPE,
    type_id   IN product_table.type_id%TYPE,
    p_name  IN product_table.product_name%TYPE,
    price  IN product_table.product_price%TYPE,
    stock IN product_table.stock%TYPE
)
IS
BEGIN
    INSERT INTO product_table(product_key, type_id, product_name, product_price, stock)
    VALUES  (pk, type_id, p_name, price, stock);
END INPUT_PRODUCT;

--INPUT_ORDER
create or replace PROCEDURE INPUT_ORDER (
    in_number       IN order_table.order_number%TYPE,
    in_ck           IN order_table.order_ck%TYPE,
    in_pk           IN order_table.order_pk%TYPE,
    in_date         IN order_table.order_date%TYPE,
    in_quantity     IN order_table.quantity%TYPE,
    in_total        IN order_table.total%TYPE
)
IS
BEGIN
    INSERT INTO order_table(order_number, order_ck, order_pk, order_date, quantity, total)
    VALUES (in_number, in_ck, in_pk, in_date, in_quantity, in_total);
END INPUT_ORDER;

--EDIT_CUSTOMER
create or replace PROCEDURE EDIT_CUSTOMER (
    ck IN CUSTOMER_TABLE.CUSTOMER_KEY%TYPE,
    clinic IN CUSTOMER_TABLE.CLINIC_NAME%TYPE,
    license IN CUSTOMER_TABLE.LICENSE_NUMBER%TYPE,
    dentist IN CUSTOMER_TABLE.DENTIST_NAME%TYPE,
    number_c IN CUSTOMER_TABLE.PHONE_NUMBER%TYPE
)
IS
BEGIN
    UPDATE customer_table c
    SET c.clinic_name = clinic,
        c.license_number = license,
        c.dentist_name = dentist,
        c.phone_number = number_c
    WHERE c.customer_key = ck;
END EDIT_CUSTOMER;

--INPUT_NOTICE
create or replace PROCEDURE INPUT_NOTICE (
    in_date IN NOTICE_TABLE.NOTICE_DATE%TYPE,
    in_notice IN NOTICE_TABLE.NOTICE_CONTENTS%TYPE
)
IS
BEGIN
    INSERT INTO notice_table(notice_date, notice_contents)
    VALUES (in_date, in_notice);
END INPUT_NOTICE;

--EDIT_PRODUCT
create or replace PROCEDURE EDIT_PRODUCT (
    e_pk IN product_table.product_key%TYPE,
    e_type_id IN product_table.type_id%TYPE,
    e_product_name IN product_table.product_name%TYPE,
    e_price IN product_table.product_price%TYPE,
    e_stock IN product_table.stock%TYPE
)
IS
BEGIN
    UPDATE product_table p
    SET p.type_id = e_type_id,
        p.product_name = e_product_name,
        p.product_price = e_price,
        p.stock = e_stock
    WHERE product_key = e_pk;
END EDIT_PRODUCT;


--EDIT_ORDER
create or replace PROCEDURE EDIT_ORDER (
    in_OrderNumber IN ORDER_TABLE.ORDER_NUMBER%TYPE,
    in_ck IN ORDER_TABLE.ORDER_CK%TYPE,
    in_pk IN ORDER_TABLE.ORDER_PK%TYPE,
    in_date IN ORDER_TABLE.ORDER_DATE%TYPE,
    in_quantity IN ORDER_TABLE.QUANTITY%TYPE,
    in_total IN ORDER_TABLE.TOTAL%TYPE
)
IS
BEGIN
    UPDATE ORDER_TABLE o
    SET o.order_ck = in_ck,
        o.order_pk = in_pk,
        o.order_date = in_date,
        o.quantity = in_quantity,
        o.total = in_total
    WHERE o.order_number = in_OrderNumber;
END EDIT_ORDER;

--ORDER_COUNT
create or replace PROCEDURE ORDER_COUNT (
    in_ck IN ORDER_TABLE.ORDER_CK%TYPE
)
IS
BEGIN
    UPDATE CUSTOMER_TABLE c
    SET c.order_count = c.order_count + 1
    WHERE c.customer_key = in_ck;
END ORDER_COUNT;

--ORDER_STOCK
create or replace PROCEDURE ORDER_STOCK (
    in_pk IN ORDER_TABLE.ORDER_PK%TYPE,
    in_quantity IN ORDER_TABLE.QUANTITY%TYPE
)
IS
BEGIN
    UPDATE PRODUCT_TABLE p
    SET p.stock = p.stock - in_quantity
    WHERE p.product_key = in_pk;
END ORDER_STOCK;




--함수

--CHECK_PRICE
create or replace FUNCTION CHECK_PRICE (pk NUMBER)
RETURN NUMBER 
IS
    price NUMBER(10);
BEGIN
    SELECT p.product_price
    INTO price
    FROM product_table p
    WHERE product_key = pk;
  RETURN price;
END CHECK_PRICE;

--CHECK_STOCK
create or replace FUNCTION CHECK_STOCK (pk NUMBER)
RETURN NUMBER 
IS
    stock NUMBER(10);
BEGIN
    SELECT p.stock
    INTO stock
    FROM product_table p
    WHERE product_key = pk;
  RETURN stock;
END CHECK_STOCK;

--CK_ORDER
create or replace FUNCTION CK_ORDER (idx NUMBER)
RETURN NUMBER 
IS
    ck NUMBER(10);
BEGIN
    select customer_key
    into ck
    from (SELECT rownum as n, customer_key
        FROM customer_table) tmp
    where tmp.n = idx;
  RETURN ck;
END CK_ORDER;

--COUNT_CUSTOMER
create or replace FUNCTION COUNT_CUSTOMER 
RETURN NUMBER 
IS
    ck_count NUMBER(10);
BEGIN
    SELECT COUNT(customer_key)
    INTO ck_count
    FROM customer_table;
  RETURN ck_count;
END COUNT_CUSTOMER;

--COUNT_PRODUCT
create or replace FUNCTION COUNT_PRODUCT
RETURN NUMBER 
IS
    pk_count NUMBER(10);
BEGIN
    SELECT COUNT(product_key)
    INTO pk_count
    FROM product_table;
  RETURN pk_count;
END COUNT_PRODUCT;


--PK_ORDER
create or replace FUNCTION PK_ORDER (idx NUMBER)
RETURN NUMBER 
IS
    pk NUMBER(10);
BEGIN
    select tmp.product_key
    into pk
    from (SELECT rownum as n, product_key
            from product_table) tmp
    where tmp.n = idx;
  RETURN pk;
END PK_ORDER;