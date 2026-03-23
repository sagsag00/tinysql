# TinySQL - Design

## `table`
I decided to create this as a class - Database which will manage everything 
related to the database, throughout the project, I added more and more functions
to make newer features be available (for example - update).

## `tokenizer`
The tokenizer was pretty straightforward to make but thinking of the design is 
what took me a bit of time, I needed a design that will always work, even if I 
scale the project. Initially I didn't make functions that should've been static
static but later I saw this mistake and fixed it, it happened with other files too.

## `parser`
Creating the parser was the hardest thing to do in this project, it took me some time.
I needed to find a way to make the engine understand string tokens, and my solution is
the ParsedQuery struct which has all the data the engine needs to know from the provided
tokens. 

## `engine`
Creating the engine was pretty easy - I just needed to translate the already parsed data 
into commands in the database. 
Throughout the project the engine had many variations.

At the same time I also changed many variables that weren't constant to constant,
fixing my previous mistakes.

## `printer`
While creating the printer, I made sure to not encapsulate functions, and I made
a wrapper function in order to make it easy for the developer and for readability.

## `test_tokenizer`
I never used gtest so I had a bit of a learning curve, but after I got the hang of it,
I managed to add the test_tokenizer and fix small issues my code had that the test highlighted.

## `test_parser`
After adding this test, more issues were highlighted and I fixed them.

## `test_engine`
The same situation as the other tests, but here, I realised that I needed each test to be independent.

## `main`
In main I just needed to implement .quit, .tables and print with the printer the result.

## `Extended`
- Order By: First I decided to add order by, which was very easy, I just needed to add a few functions
- Update
- .save/.load
