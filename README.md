This folder contains a few differnet prototypes demonstrating ideas for possible ways to structure the code
  - OOP message passing observer pattern
      - The biggest main idea is object oriented message passing and module/class isolation. We can separate the application 
        into modules (probably corresponding to each device the ccm communicates with but other organizations might be more effective). 
        We can enforce isolation between the states of the modules by only allowing them to communicate through message passing.
        This is essentially what the observer pattern does, with the messages being the notify() function
      - In a general sense, OOP encapsulates the state of objects by making fields private and allowing communicatoin only through
        methods, but to make things even more isolated with the observer pattern, we can enforce that each class solely communicates through
        the notify() function. This also allows us to associate 1 thread per class and use asynchronous channels to send messages. All
        messages to a class must be received in 1 spot and then handled from there
      - In a threaded channel based approach, we will use dynamic dispatch to send messages of multiple types to one channel. In a single
        threaded superloop approach we can instead statically dispatch messages using function overloading

  - Procedural approach
      - An alternative to the OOP observer pattern is to simply write code procedurally. Classes can still be used, but used conservatively
      - A principle I like to follow is state invariants. The fundemental idea of OOP is encapsulation through message passing (method calls).
        An imperative program can be viewed as a state, along with transitions you can take from one state to another state. In the meantime the
        program can have outputs based on the current state and transitoins. Imperative programs are erroneous when state invariants are violated,
        and we enter into an invalid state that doesn't make sense. Objects help us by maintaining the state invariants by encapsulating some
        intertwined state in an opaque black box, and only methods to access this black box that are guaranteed to not break the invariants. Good
        OOP abstractions are abstractions where Objects group together state that makes perfect sense to be grouped together, and the object is
        properly treated as a black box. Perfect examples are data structures such as a linked list, hashmap, or priority queue. However, these
        abstractions are often very hard to come up with, and there is a cost to building these walls because the moment you realize the walls are
        not well designed, refactoring by tearing down the walls is expensive. I think the OOP world is bloated with tons of crappy abstractions that 
        make it feel like you are doing something but in reality all of your classes point to all the other classes without structure, classes reach into
        other classes randomly because you realize you need to break random arbitrary walls that you built up earlier since they aren't good abstractions.
        And chaining a ridiculous amount of method calls because you built so many unnecessary walls. Things can seem like good abstractions on a high 
        level at first, but once things get more complex things can fall apart. So I would suggest being cautious about not building too many barriers.
        I personally don't know what the application is doing very well, so I struggled to think about what the walls should be
      - A video that explains my concerns very well: https://www.youtube.com/watch?v=QM1iUe6IofM 
      - To avoid cross-barrier interaction, it might be best to sometimes not associate functions with data unnecessarily. Sometimes a function can
        just be a function and data can just be data.
      - Functions themselves can become the levels of abstraction that modularize the code

  - Downsides of OOP
      - Part of the reason why it took me so long to make this prototype is because I am very agnostic about OOP.
          - The main principle of OOP according to its founder was the idea of message passing, and I think the issue
            with this idea is that there is a right way and a wrong way to interpret and reify the idea. If you interpret the idea as a
            computer scientist trying to minimize the effects of shared state breaking the invariants of a program you can create good
            abstractions that actually help with that cause, but there is a pervasive tendency to think of OOP in high level abstractions
            that are blind to the actual code. On the extreme end for example, when I took AP Computer Science, Java classes were taught
            using animal analogies, like Cat and Dog inherit from Animal. This makes perfect sense on a high level and makes you feel good
            when you draw diagrams and connect objects together in a way that resembles real life, and the less ridiculous examples like
            visitor, comand, and observer are also very much analogous to real life high level thinking, but this is not how code actually works.


util/
- asynchronous channel implementation, option/result implementation

procedural/
- procedural superloop implementation

messaging/
- thread message passing implementation

observer_dynamic_dispatch/


move_semantics.cpp
- a tutorial on move semantics