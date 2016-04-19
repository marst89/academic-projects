Implementation of a DBMS consisting of 4 layers in C.
The 1rst layer implements an Application Programming Interface (API) allowing the communication 
of the above layers with the disk using the block (the page) as the communication unit.
The 2nd layer provides the functionality of an index, based on the Extensible hashing structure.
The 3rd layer provides to users the capability of creating a db, deleting a db, creating a relationship,
building an index on a relationship’s attribute, destroying a relationship, and manipulating data using
the select, delete, and insert operators. 
The 4th layer implements an interface for the treatment of materialized views. (Software Development, 2009)

Authors:
Maria Poutachidou
Christos Karaiskos
Dimitra Tsirikou