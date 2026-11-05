#include <iostream>
#include <string>
#include <cctype>
#include <cmath>

using namespace std;

// ============================================================================
// PART 1: CUSTOM STACK IMPLEMENTATION (LINKED LIST BASED)
// ============================================================================
template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val, Node* ptr = nullptr) : data(val), next(ptr) {}
    };

    Node* head;
    size_t current_size;

public:
    Stack() : head(nullptr), current_size(0) {}

    ~Stack() {
        clear();
    }

    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    void push(const T& value) {
        head = new Node(value, head);
        current_size++;
    }

    bool pop(T& value) {
        if (isEmpty()) return false;
        Node* temp = head;
        value = head->data;
        head = head->next;
        delete temp;
        current_size--;
        return true;
    }

    bool peek(T& value) const {
        if (isEmpty()) return false;
        value = head->data;
        return true;
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    size_t size() const {
        return current_size;
    }

    void clear() {
        T temp;
        while (pop(temp));
    }
};

// ============================================================================
// PART 2: BRACKET VALIDATION
// ============================================================================
bool matches(char open, char close) {
    return (open == '(' && close == ')') ||
           (open == '[' && close == ']') ||
           (open == '{' && close == '}');
}

bool checkBrackets(const string& expr, string& errorMsg) {
    Stack<char> s;
    for (size_t i = 0; i < expr.length(); ++i) {
        char ch = expr[i];
        if (ch == '(' || ch == '[' || ch == '{') {
            s.push(ch);
        } else if (ch == ')' || ch == ']' || ch == '}') {
            char top;
            if (!s.pop(top)) {
                errorMsg = "Unmatched closing bracket '" + string(1, ch) + "'";
                return false;
            }
            if (!matches(top, ch)) {
                errorMsg = "Mismatched bracket pair: '" + string(1, top) + "' and '" + string(1, ch) + "'";
                return false;
            }
        }
    }
    if (!s.isEmpty()) {
        errorMsg = "Unclosed opening bracket(s) remaining";
        return false;
    }
    return true;
}

// ============================================================================
// PART 3: TOKENIZATION & INFIX TO POSTFIX CONVERSION
// ============================================================================
int getPrecedence(char op) {
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

bool isOperator(char ch) {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

bool convertToPostfix(const string& expr, string& postfix, string& errorMsg) {
    Stack<char> opStack;
    postfix = "";
    bool expectOperand = true;

    size_t i = 0;
    while (i < expr.length()) {
        char ch = expr[i];

        if (isspace(ch)) {
            i++;
            continue;
        }

        if (!isdigit(ch) && !isOperator(ch) &&
            ch != '(' && ch != ')' && ch != '[' && ch != ']' && ch != '{' && ch != '}') {
            errorMsg = "Unsupported character: '" + string(1, ch) + "'";
            return false;
        }

        if (isdigit(ch)) {
            if (!expectOperand) {
                errorMsg = "Missing operator between numbers/groups";
                return false;
            }
            string num = "";
            while (i < expr.length() && isdigit(expr[i])) {
                num += expr[i];
                i++;
            }
            postfix += num + " ";
            expectOperand = false;
            continue;
        }

        if (ch == '(' || ch == '[' || ch == '{') {
            if (!expectOperand) {
                errorMsg = "Implicit multiplication is not permitted";
                return false;
            }
            opStack.push(ch);
            expectOperand = true;
        } else if (ch == ')' || ch == ']' || ch == '}') {
            if (expectOperand) {
                errorMsg = "Empty group or misplaced closing bracket";
                return false;
            }
            char top;
            while (opStack.peek(top) && top != '(' && top != '[' && top != '{') {
                opStack.pop(top);
                postfix += string(1, top) + " ";
            }
            opStack.pop(top);
            expectOperand = false;
        } else if (isOperator(ch)) {
            if (expectOperand) {
                errorMsg = "Invalid operator sequence or leading unary sign";
                return false;
            }
            char top;
            while (opStack.peek(top) && isOperator(top) && getPrecedence(top) >= getPrecedence(ch)) {
                opStack.pop(top);
                postfix += string(1, top) + " ";
            }
            opStack.push(ch);
            expectOperand = true;
        }
        i++;
    }

    if (expectOperand) {
        errorMsg = "Expression ends with a trailing operator or empty input";
        return false;
    }

    char top;
    while (opStack.pop(top)) {
        postfix += string(1, top) + " ";
    }

    if (!postfix.empty() && postfix.back() == ' ') {
        postfix.pop_back();
    }

    return true;
}

// ============================================================================
// PART 4: POSTFIX EVALUATION
// ============================================================================
bool evaluatePostfix(const string& postfix, double& result, string& errorMsg) {
    Stack<double> valStack;
    size_t i = 0;

    while (i < postfix.length()) {
        if (isspace(postfix[i])) {
            i++;
            continue;
        }

        if (isdigit(postfix[i])) {
            double val = 0;
            while (i < postfix.length() && isdigit(postfix[i])) {
                val = val * 10 + (postfix[i] - '0');
                i++;
            }
            valStack.push(val);
            continue;
        }

        if (isOperator(postfix[i])) {
            char op = postfix[i];
            double right, left;
            if (!valStack.pop(right) || !valStack.pop(left)) {
                errorMsg = "Insufficient operands for operator '" + string(1, op) + "'";
                return false;
            }

            if (op == '/' && right == 0.0) {
                errorMsg = "Division by zero";
                return false;
            }

            if (op == '+') valStack.push(left + right);
            else if (op == '-') valStack.push(left - right);
            else if (op == '*') valStack.push(left * right);
            else if (op == '/') valStack.push(left / right);

            i++;
        }
    }

    if (!valStack.pop(result) || !valStack.isEmpty()) {
        errorMsg = "Malformed postfix evaluation";
        return false;
    }

    return true;
}

// ============================================================================
// PART 5: DEMO & MENU INTERFACE
// ============================================================================
void runStackDemo() {
    cout << "\n--- STACK DEMONSTRATION ---\n";
    Stack<int> s;
    int val;

    cout << "1. pop() on empty: " << (s.pop(val) ? "true" : "false") << " (size: " << s.size() << ")\n";

    s.push(10);
    s.push(20);
    s.peek(val);
    cout << "2. push(10), push(20), peek(): " << val << " (size: " << s.size() << ")\n";

    s.push(30);
    cout << "3. push(30) (size: " << s.size() << ")\n";

    cout << "4. Popping items: ";
    while (s.pop(val)) {
        cout << val << " ";
    }
    cout << "\n5. peek() on empty: " << (s.peek(val) ? "true" : "false") << " (size: " << s.size() << ")\n";
}

void processExpression(const string& expr) {
    cout << "\nInput Expression: " << expr << "\n";

    string bracketError;
    if (!checkBrackets(expr, bracketError)) {
        cout << "[Step 2 Bracket Check]: REJECTED -> " << bracketError << "\n";
        return;
    }
    cout << "[Step 2 Bracket Check]: Balanced PASSED\n";

    string postfix, parseError;
    if (!convertToPostfix(expr, postfix, parseError)) {
        cout << "[Step 3 & 4 Conversion]: REJECTED -> " << parseError << "\n";
        return;
    }
    cout << "Postfix Representation: " << postfix << "\n";

    double result;
    string evalError;
    if (!evaluatePostfix(postfix, result, evalError)) {
        cout << "[Step 5 Evaluation]: REJECTED -> " << evalError << "\n";
        return;
    }
    cout << "Evaluated Result: " << result << "\n";
}

int main() {
    while (true) {
        cout << "\n===========================================\n";
        cout << "           CC105 STACK EVALUATOR           \n";
        cout << "===========================================\n";
        cout << "1. Check brackets\n";
        cout << "2. Convert and evaluate expression\n";
        cout << "3. Run stack demonstration\n";
        cout << "0. Exit\n";
        cout << "Choice: ";

        string inputChoice;
        if (!getline(cin, inputChoice)) break;

        if (inputChoice == "1") {
            cout << "Enter expression: ";
            string expr, err;
            getline(cin, expr);
            if (checkBrackets(expr, err)) {
                cout << "Result: Brackets are BALANCED.\n";
            } else {
                cout << "Result: REJECTED (" << err << ")\n";
            }
        } else if (inputChoice == "2") {
            cout << "Enter expression: ";
            string expr;
            getline(cin, expr);
            processExpression(expr);
        } else if (inputChoice == "3") {
            runStackDemo();
        } else if (inputChoice == "0") {
            cout << "Exiting application...\n";
            break;
        } else {
            cout << "Invalid menu selection. Please try again.\n";
        }
    }
    return 0;
}
