
#include <iostream>
#include <stdexcept>
#include <string>
#include <cmath>
#include <map>

//template from Professor kaiser, my main goal is to implentment four feathures
//1.negative numbers 2.% 3.pre-defined symbolic values 4.Variables 
//with additional comprehensiive test

//update:
//either main () conflict with catch.cpp or 
//in catch.cpp putback() into full buffer


// Token stuff
// Token “kind” values:
char const number = '8';        // a floating-point number
char const quit = 'q';          // an exit command
char const print = ';';         // a print command
char const Var='x';             //4.variables
char const variable_assign='='; //help to assign variable 

//3. pre-defined symbolic values
const double pi_value=3.141592653;  // like 2*pi 
const double e_value=2.7182818284;  // like e^3

 
class token
{
    char kind_;       // what kind of token
    double value_;    // for numbers: a value
    std::string Var_; //for variables

public:
    // constructors
    token(char ch)
      : kind_(ch)
      , value_(0)
    {
    }
    token(double val)
      : kind_(number)    // let ‘8’ represent “a number”
      , value_(val)
      
    {
    }
    token (char ch, double val)
        :kind_(ch)
        ,value_(val)
    {

    }
    token(char ch, std::string Var)
        :kind_(ch)      //let x represent a varaible 
        ,Var_(Var)
        ,value_(0)
    {
    }
    char kind() const
    {
        return kind_;
    }
    double value() const
    {
        return value_;
    }
    std::string Var()const
    {
        return Var_;
    }
    
};

// User interaction strings:
std::string const prompt = "> ";
std::string const result = "= ";    // indicate that a result follows

class token_stream
{
    // representation: not directly accessible to users:
    bool full;       // is there a token in the buffer?
    token buffer;    // here is where we keep a Token put back using
                     // putback()
public:
    // user interface:
    token get();            // get a token
    void putback(token);    // put a token back into the token_stream
    void ignore(char c);    // discard tokens up to and including a c

    // constructor: make a token_stream, the buffer starts empty
    token_stream()
      : full(false)
      , buffer('\0')
    {
    }
};

// single global instance of the token_stream
token_stream ts;
std::map<std::string, double> variables;

void token_stream::putback(token t)
{
    if (full)
        throw std::runtime_error("putback() into a full buffer");
    buffer = t;
    full = true;
}

token token_stream::get()    // read a token from the token_stream
{
    // check if we already have a Token ready
    if (full)
    {
        full = false;
        return buffer;
    }

    // note that >> skips whitespace (space, newline, tab, etc.)
    char ch;
    std::cin >> ch;

    switch (ch)
    {
    case '(':
    case ')':
    case ';':
    case 'q':
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '=':
    case '^':
            return token(ch);    // let each character represent itself
    case '.':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
        std::cin.putback(ch);    // put digit back into the input stream
        double val;
        std::cin >> val;    // read a floating-point number
        return token(val);
    }
    case 'p':
    {
        // check for exactly wriiten "pi"
        if (std::cin.peek() == 'i')
        {
            std::cin.get();
            return token(number, pi_value);
        }
        else
        {   //throw an error if not pi
            throw std::runtime_error("Bad token");
        }
    }
    case 'e':
        //throw error if ei, ea, ep;
        if(std::cin.peek() != '^' && std::cin.peek() != '*'&& std::cin.peek() != '%'
        && std::cin.peek() != '+'&& std::cin.peek() != '-')
            throw std::runtime_error("Bad token");
        return token(number, e_value);
        break;
    default:
        //function isaplha check is it aplhabetic  --the character
        if (isalpha(ch))
        {
            std::string var_character;
            var_character+=ch;
            //read whole variable Var
            while (std::cin.get(ch))
            var_character+=ch;
            std::cin.putback(ch);

            if (var_character=="pi") 
                return token (number, pi_value);

            else if (var_character=="e") 
                return token (number, e_value);

            else return token(Var, var_character);
        }
        throw std::runtime_error("Bad token");
    }
}

// discard tokens up to and including a c
void token_stream::ignore(char c)
{
    // first look in buffer:
    if (full && c == buffer.kind())    // && means 'and'
    {
        full = false;
        return;
    }
    full = false;    // discard the contents of buffer

    // now search input:
    char ch = 0;
    while (std::cin >> ch)
    {
        if (ch == c)
            break;
    }
}

// declaration so that primary() can call expression()
double expression();

double primary()    // Number or ‘(‘ Expression ‘)’
{
    token t = ts.get();
    switch (t.kind())
    {
    case '(':    // handle ‘(’expression ‘)’
    {
        double d = expression();
        t = ts.get();
        if (t.kind() != ')')
            throw std::runtime_error("')' expected");
        return d;
    }
    case number:    // we use ‘8’ to represent the “kind” of a number
        return t.value();    // return the number’s value
    case '-':       //for negative numbers
        return -primary();   //return the negative numbers
    case '^':
        {
            double d =primary();
            return std::pow (t.value(), d);  //exponent function, t.value^d
        }
    case Var:
        {
            token next_t = ts.get();
            if (next_t.kind() == variable_assign)  //if next is "="
            {
            double value = expression();
            variables[t.Var()] = value;
                return value;
            } 
            else if (variables.count(t.Var())) 
            {
                ts.putback(next_t);
                return variables[t.Var()];
            }

            else 
            {
            throw std::runtime_error("undefined variable");
            }
        }
    default:
        throw std::runtime_error("primary expected");
    }
}

// exactly like expression(), but for * and /
double term()
{
    double left = primary();    // get the Primary
    while (true)
    {
        token t = ts.get();    // get the next Token ...
        switch (t.kind())
        {
        case '*':
            left *= primary();
            break;
        case '/':
        {
            double d = primary();
            if (d == 0)
                throw std::runtime_error("divide by zero");
            left /= d;
            break;
        }
        case '%':
        {
            double d=primary();
            if (d==0)
                throw std::runtime_error("divide by zero");
            left=left-d*static_cast<int>(left/d); 
            // formula remainder=dividend - divisor * (dividend / divisor)
            break;
        }
        case '^':
        {
            double d= primary();
            left=std::pow(left,d);
            break;
        }
        case '=':
        {
            left=primary();
            break;
        }
        default:
            ts.putback(t);    // <<< put the unused token back
            return left;      // return the value
        }
    }
}

// read and evaluate: 1   1+2.5   1+2+3.14  etc.
// 	 return the sum (or difference)
double expression()
{
    double left = term();    // get the Term
    while (true)
    {
        token t = ts.get();    // get the next token…
        switch (t.kind())      // ... and do the right thing with it
        {
        case '+':
            left += term();
            break;
        case '-':
            left -= term();
            break;
        case '*':
            left*=term();
            break;
        case '=':
            left=term();
            break;
        default:
            ts.putback(t);    // <<< put the unused token back
            return left;      // return the value of the expression
        }
    }
}

void clean_up_mess()
{
    ts.ignore(print);
}

void calculate()
{
    while (std::cin)
    {
        try
        {
            std::cout << prompt;    // print prompt
            token t = ts.get();

            // first discard all “prints”
            while (t.kind() == print)
                t = ts.get();

            if (t.kind() == quit)
                return;    // ‘q’ for “quit”
            if (t.kind()==print)
                continue;

            ts.putback(t);

            std::cout << result << expression() << std::endl;
        }
        catch (std::runtime_error const& e)
        {
            std::cerr << e.what() << std::endl;    // write error message
            clean_up_mess();                       // <<< The tricky part!
        }
    }
}

int main()
{
   try
    {
        calculate();
        return 0;
    }
    catch (...)
    {
        // other errors (don't try to recover)
        std::cerr << "exception\n";
       return 2;
    }
}
