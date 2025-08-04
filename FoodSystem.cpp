#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#define MAX 1000
#define MAX_HOTELS 10
#define MAX_OPTIONS 100
#define MAX_ORDERS 100
using namespace std;
class Entity
{
public:
    virtual ~Entity() {}
};
class FoodItem : public Entity
{
public:
    char name[100];
    int price;
};
class User : public Entity
{
public:
    char username[50];
    char password[50];
    int userId;
    bool isAdmin;
};
class Hotel : public Entity
{
public:
    char name[100];
    FoodItem vegOpt[MAX_OPTIONS];
    FoodItem nonVegOpt[MAX_OPTIONS];
    FoodItem bev[MAX_OPTIONS];
    int vegCount, nonVegCount, bevCount;
};
class Order
{
public:
    int orderId;
    int userId;
    char hotelName[100];
    FoodItem items[MAX_OPTIONS];
    int quantities[MAX_OPTIONS];
    int itemCount;
    bool completed;
    Order() : itemCount(0), completed(false) {}
    int getTotal()const
    {
        int total=0;
        for(int i=0;i<itemCount;i++)
        {
            total+=items[i].price*quantities[i];
        }
        return total;
    }
};
class OrderQueue
{
private:
    Order orders[MAX_ORDERS];
    int front;
    int rear;
    int count;
public:
    OrderQueue() : front(0), rear(-1), count(0) {}
    bool isEmpty()const
    {
        return count==0;
    }
    bool isFull()const
    {
        return count==MAX_ORDERS;
    }
    void enqueue(const Order& order)
    {
        if(isFull())
        {
            cout<<"Order queue is full. Cannot accept more orders.\n";
            return;
        }
        rear=(rear+1) % MAX_ORDERS;
        orders[rear]=order;
        count++;
    }
    Order dequeue()
    {
        if(isEmpty())
        {
            cout<<"No orders to process.\n";
            return Order();
        }
        Order order=orders[front];
        front=(front+1) % MAX_ORDERS;
        count--;
        return order;
    }
    Order peek()const
    {
        if(isEmpty())
        {
            cout<<"No orders in queue.\n";
            return Order();
        }
        return orders[front];
    }
    void displayAll()const
    {
        if(isEmpty())
        {
            cout<<"No pending orders!\n";
            return;
        }
        int current=front;
        int orderNum=1;
        for(int i=0;i<count;i++)
        {
            Order o=orders[current];
            cout<<"\nOrder #"<<orderNum++<<" (User ID: "<< o.userId<<")\n";
            cout<<"Restaurant: "<<o.hotelName<<endl;
            cout<<"Items:\n";
            for(int j=0;j<o.itemCount;j++)
            {
                cout<<"- "<<o.items[j].name<<" x"<<o.quantities[j]<<" ($" << o.items[j].price*o.quantities[j]<<")\n";
            }
            cout<<"Total: $" <<o.getTotal()<<endl;
            current = (current+1) % MAX_ORDERS;
        }
    }
};
class UserManagement 
{
protected:
    User u[MAX];
    int userCount;
    const char* userFile = "users.txt";
    void loadUsers() 
    {
        ifstream file(userFile);
        if(file) 
        {
            userCount=0;
            while(file>>u[userCount].username>>u[userCount].password>>u[userCount].userId>>u[userCount].isAdmin) 
            {
                userCount++;
            }
            file.close();
        } 
        else
        {
            userCount = 0;
        }
    }
    void saveUsers() 
    {
        ofstream file(userFile);
        for (int i=0;i<userCount;i++) 
        {
            file<<u[i].username<<" "<<u[i].password<<" "<<u[i].userId<<" "<<u[i].isAdmin<<endl;
        }
        file.close();
    }
public:
    UserManagement() 
    {
        loadUsers();
        if (userCount==0) 
        {
            strcpy(u[0].username,"Admin_priya");
            strcpy(u[0].password,"130204");
            u[0].userId=100;
            u[0].isAdmin=true;
            userCount=1;
            saveUsers();
        }
    }
    bool registerUser() 
    {
        User newUser;
        cout<<"Enter username: ";
        cin>>newUser.username;
        for(int i=0;i<userCount;i++) 
        {
            if(strcmp(u[i].username,newUser.username)==0) 
            {
                cout<<"\nUsername already exists! Try again with a new username.";
                return false;
            }
        }
        cout<<"Enter password: ";
        cin>>newUser.password;
        newUser.userId=1000+userCount;
        newUser.isAdmin=false;
        u[userCount]=newUser;
        userCount++;
        saveUsers();
        cout<<"\nRegistration successful! \nYour User ID: "<<newUser.userId<<endl;
        return true;
    }
    User* loginUser() 
    {
        char username[50],password[50];
        cout<<"Username: ";
        cin>>username;
        cout<<"Password: ";
        cin>>password;
        for(int i=0;i<userCount;i++) 
        {
            if(strcmp(u[i].username, username)==0 && strcmp(u[i].password, password)==0) 
            {
                cout<<"\nLogin successful! \nWelcome, "<<username<<"!";
                return &u[i];
            }
        }
        cout<<"\nInvalid username or password!";
        return nullptr;
    }
};
class FoodSystem : public UserManagement 
{
private:
    Hotel hotels[MAX_HOTELS];
    int hotelCount;
    OrderQueue orderQueue;
    void loadHotels() 
    {
        ifstream file("hotels.txt");
        if(!file.is_open()) 
        {
            cout<<"Error: Unable to open hotels file.\n";
            return;
        }
        hotelCount=0;
        string line;
        while (getline(file, line) && hotelCount < MAX_HOTELS) 
        {
            if (line.empty()) continue;
            Hotel currHotel;
            strncpy(currHotel.name, line.c_str(), sizeof(currHotel.name) - 1);
            currHotel.name[sizeof(currHotel.name) - 1]='\0';
            currHotel.vegCount=0;
            currHotel.nonVegCount=0;
            currHotel.bevCount=0;
            string category;
            while(getline(file,line) && !line.empty()) 
            {
                if(line=="Vegetarian:") 
                {
                    category="Vegetarian";
                } 
                else if(line=="Non-Vegetarian:") 
                {
                    category="Non-Vegetarian";
                } 
                else if(line=="Beverages:") 
                {
                    category="Beverages";
                }
                else 
                {
                    size_t colonPos = line.find(':');
                    if (colonPos < line.length())
                    {
                        string itemName = line.substr(0, colonPos);
                        string priceStr=line.substr(colonPos + 1);
                        int price=stoi(priceStr);
                        FoodItem item;
                        strncpy(item.name, itemName.c_str(), sizeof(item.name) - 1);
                        item.name[sizeof(item.name) - 1]='\0';
                        item.price=price;
                        if(category=="Vegetarian" && currHotel.vegCount<MAX_OPTIONS) 
                        {
                            currHotel.vegOpt[currHotel.vegCount++] = item;
                        } 
                        else if(category=="Non-Vegetarian" && currHotel.nonVegCount<MAX_OPTIONS) 
                        {
                            currHotel.nonVegOpt[currHotel.nonVegCount++]=item;
                        } 
                        else if(category=="Beverages" && currHotel.bevCount<MAX_OPTIONS) 
                        {
                            currHotel.bev[currHotel.bevCount++] = item;
                        }
                    }
                }
            }
            hotels[hotelCount++]=currHotel;
        }
        file.close();
    }
public:
    FoodSystem() : UserManagement() 
    {
        loadHotels();
    }
    void showMainMenu() 
    {
        int ch;
        do {
            cout<<"\nFOOD ORDERING SYSTEM\n";
            cout<<"1. Register\n";
            cout<<"2. Login\n";
            cout<<"3. Admin Login\n";
            cout<<"4. Exit\n";
            cout<<"Enter choice: ";
            cin>>ch;
            User* currentUser=nullptr;
            switch(ch) 
            {
                case 1:
                    registerUser();
                    break;
                case 2:
                    currentUser=loginUser();
                    if (currentUser && !currentUser->isAdmin) 
                    {
                        showFoodHotels();
                    }
                    break;
                case 3:
                    currentUser=loginUser();
                    if(currentUser && currentUser->isAdmin) 
                    {
                        adminPanel();
                    }
                    break;
                case 4:
                    exit(0);
                default:
                    cout<<"\nInvalid choice!";
            }
        }while(true);
    }
private:
    void adminPanel() 
    {
        int choice;
        do 
        {
            cout<<"\nADMIN PANEL\n";
            cout<<"1. View Orders\n";
            cout<<"2. Process Order\n";
            cout<<"0. Exit\n";
            cout<<"Enter choice: ";
            cin>>choice;
            switch (choice) 
            {
                case 1:
                    orderQueue.displayAll();
                    break;
                case 2:
                    processOrder();
                    break;
                case 0:
                    return;
                default:
                    cout<<"Invalid choice!\n";
            }
        }while(true);
    }
    void processOrder() 
    {
        if (orderQueue.isEmpty()) 
        {
            cout<<"\nNo orders to process!\n";
            return;
        }
        Order o=orderQueue.peek();
        cout<<"\nProcessing Order:\n";
        cout<<"Restaurant: "<<o.hotelName<<endl;
        cout<<"Items:\n";
        for(int i=0;i<o.itemCount;i++) 
        {
            cout<<"- "<<o.items[i].name<<" x"<<o.quantities[i]<<endl;
        }
        cout<<"Total: $"<<o.getTotal()<<endl;
        cout<<"Confirm processing? (1=Yes, 0=No): ";
        int confirm;
        cin>>confirm;
        if(confirm==1) 
        {
            orderQueue.dequeue();
            cout<<"Order processed successfully!\n";
        }
    }
    void showFoodHotels() 
    {
        cout<<"\nAvailable Restaurants:\n";
        for(int i=0;i<hotelCount;i++) 
        {
            cout<<i + 1<<". "<<hotels[i].name<<endl;
        }
        selectFoodHotel();
    }
    void selectFoodHotel() 
    {
        int choice;
        cout<<"\nSelect a restaurant by number: ";
        cin>>choice;
        if(choice>=1 && choice<=hotelCount) 
        {
            Hotel& selectedHotel=hotels[choice - 1];
            cout<<"\nYou selected: "<<selectedHotel.name<<endl;
            chooseFoodType(selectedHotel);
        } 
        else 
        {
            cout<<"\nInvalid selection! Please try again.";
        }
    }
    void chooseFoodType(Hotel& selectedHotel) 
    {
        Order currentOrder;
        strncpy(currentOrder.hotelName, selectedHotel.name, sizeof(currentOrder.hotelName)-1);
        currentOrder.hotelName[sizeof(currentOrder.hotelName)-1]='\0';
        int choice;
        do 
        {
            cout<<"\nMenu Categories:\n";
            cout<<"1. Vegetarian\n";
            cout<<"2. Non-Vegetarian\n";
            cout<<"3. Beverages\n";
            cout<<"4. View Cart\n";
            cout<<"5. Checkout\n";
            cout<<"0. Back to Restaurants\n";
            cout<<"Enter choice: ";
            cin>>choice;
            switch (choice) 
            {
                case 1:
                    showOptions(selectedHotel.vegOpt, selectedHotel.vegCount);
                    orderItems(selectedHotel.vegOpt, selectedHotel.vegCount, currentOrder);
                    break;
                case 2:
                    showOptions(selectedHotel.nonVegOpt, selectedHotel.nonVegCount);
                    orderItems(selectedHotel.nonVegOpt, selectedHotel.nonVegCount, currentOrder);
                    break;
                case 3:
                    showOptions(selectedHotel.bev, selectedHotel.bevCount);
                    orderItems(selectedHotel.bev, selectedHotel.bevCount, currentOrder);
                    break;
                case 4:
                    viewCart(currentOrder);
                    break;
                case 5:
                    checkout(currentOrder);
                    return;
                case 0:
                    return;
                default:
                    cout<<"\nInvalid choice!";
                    break;
            }
        } while (true);
    }
    void showOptions(FoodItem options[MAX_OPTIONS], int count) 
    {
        cout<<"\nAvailable Options:\n";
        for(int i=0;i<count;i++) 
        {
            cout<<i + 1<<". "<<options[i].name<<" - Price: $"<<options[i].price<<endl;
        }
    }
    void orderItems(FoodItem options[MAX_OPTIONS],int count,Order& currentOrder) 
    {
        int choice,quantity;
        cout<<"Enter item number to order (0 to finish this category): ";
        cin>>choice;
        while (choice!=0) 
        {
            if (choice>=1 && choice<=count) 
            {
                cout<<"Enter quantity: ";
                cin>>quantity;
                if (quantity>0) 
                {
                    if (currentOrder.itemCount < MAX_OPTIONS) 
                    {
                        currentOrder.items[currentOrder.itemCount]=options[choice-1];
                        currentOrder.quantities[currentOrder.itemCount]=quantity;
                        currentOrder.itemCount++;
                        cout<<"Added to cart!\n";
                    } 
                    else 
                    {
                        cout<<"Cart is full! Cannot add more items.\n";
                    }
                } 
                else 
                {
                    cout<<"Quantity must be positive!\n";
                }
            } 
            else 
            {
                cout<<"Invalid item number!\n";
            }
            cout<<"Enter item number to order (0 to finish this category): ";
            cin>>choice;
        }
    }
    void viewCart(Order& currentOrder) 
    {
        if(currentOrder.itemCount==0) 
        {
            cout<<"\nYour cart is empty!\n";
            return;
        }
        cout<<"\nYour Cart ("<<currentOrder.hotelName<<"):\n";
        int total=0;
        for(int i=0;i<currentOrder.itemCount;i++) 
        {
            cout<<"- "<<currentOrder.items[i].name<<" x"<<currentOrder.quantities[i]<<" ($"<<currentOrder.items[i].price * currentOrder.quantities[i]<<")\n";
            total+=currentOrder.items[i].price * currentOrder.quantities[i];
        }
        cout<<"Total: $"<<total<<endl;
    }
    void checkout(Order& currentOrder) 
    {
        if(currentOrder.itemCount==0) 
        {
            cout<<"\nYour cart is empty!\n";
            return;
        }
        cout<<"\nOrder Summary:\n";
        cout<<"Restaurant: "<<currentOrder.hotelName<<endl;
        int total=0;
        for(int i=0;i<currentOrder.itemCount;i++) 
        {
            cout<<"- "<<currentOrder.items[i].name<<" x"<<currentOrder.quantities[i]<<" ($"<<currentOrder.items[i].price * currentOrder.quantities[i]<<")\n";
            total+=currentOrder.items[i].price * currentOrder.quantities[i];
        }
        cout<<"Total: $"<<total<<endl;
        cout<<"Confirm order? (1=Yes, 0=No): ";
        int confirm;
        cin>>confirm;
        if(confirm==1) 
        {
            orderQueue.enqueue(currentOrder);
            cout<<"Order placed successfully!\n";
        }
    }
};
int main() 
{
    FoodSystem foodSystem;
    foodSystem.showMainMenu();
    return 0;
}