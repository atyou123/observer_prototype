#ifndef FALLIBLE_H
#define FALLIBLE_H

struct IllegalUnwrap {};

template <typename T> class Option {
  public:
    enum Flag {
        Some,
        None,
    };

    static Option MakeSome(T value) {
        Option opt;
        opt._flag = Some;
        opt._value = value;
        return opt;
    }

    static Option MakeNone() {
        Option opt;
        opt._flag = None;
        return opt;
    }

    Flag GetFlag() { return _flag; }

    T Unwrap() {
        switch (_flag) {
        case Some:
            return _value;
        case None:
            throw IllegalUnwrap();
        }
    }

  private:
    // Unspecified if _flag == None
    T _value;

    Flag _flag;
    Option() {}
};

template <typename T, typename E> class Result {
  public:
    enum Flag {
        Ok,
        Err,
    };

    static Result MakeSome(T value) {
        Result res;
        res._flag = Ok;
        res._value = value;
        return res;
    }

    static Result MakeNone(E error) {
        Result res;
        res._flag = Err;
        res._error = error;
        return res;
    }

    Flag GetFlag() { return _flag; }

    T UnwrapOk() {
        switch (_flag) {
        case Ok:
            return _value;
        case Err:
            throw IllegalUnwrap();
        }
    }

    E UnwrapErr() {
        switch (_flag) {
        case Ok:
            throw IllegalUnwrap();
        case Err:
            return _error;
        }
    }

  private:
    // T unspecified if flag is Err
    // E unspecified if flag is Ok
    union {
        T _value;
        E _error;
    };
    Flag _flag;
    Result() {}
};

#endif
