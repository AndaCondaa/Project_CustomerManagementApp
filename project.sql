--CREATE customer table
CREATE TABLE customer (
	customer_key number(10) NOT NULL,
	clinic_name varchar2(20) NOT NULL,
	license_number varchar2(10) NOT NULL,
	dentist_name varchar2(10) NOT NULL,
	phone_number varchar2(15),
    order_amount number(10),
	PRIMARY KEY(customer_key)
);

SELECT *
FROM customer;

delete from customer;

--add procedure
CREATE OR REPLACE PROCEDURE INPUT_CUSTOMER
(
    ck       IN CUSTOMER.CUSTOMER_KEY%TYPE,
    clinic   IN CUSTOMER.CLINIC_NAME%TYPE,
    dentist  IN CUSTOMER.LICENSE_NUMBER%TYPE,
    license  IN CUSTOMER.DENTIST_NAME%TYPE,
    number_c IN CUSTOMER.PHONE_NUMBER%TYPE,
    amount   IN CUSTOMER.ORDER_AMOUNT%TYPE 
)
IS
BEGIN
    INSERT INTO CUSTOMER(CUSTOMER_KEY, CLINIC_NAME, LICENSE_NUMBER, DENTIST_NAME, PHONE_NUMBER, ORDER_AMOUNT)
    VALUES  (ck, clinic, license, dentist, number_c, amount);
END INPUT_CUSTOMER;
/
commit;