#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

struct BankAccount
{
  int balance = 0;

  BankAccount() = default;

  explicit BankAccount(const int balance)
    : balance{balance}
  {

  }
  void deposit(int amount)
  {
    balance += amount;
  }
  bool withdraw(int amount)
  {
    bool result = false;
    if (amount <= balance)
    {
      balance -= amount;
      result = true;
    }
    return result;
  }
};

// Fixture 1
struct BankAccountTest : testing::Test
{
  BankAccount* account;

  BankAccountTest()
  {
    account = new BankAccount;
  };

  virtual ~BankAccountTest()
  {
    delete account;
  }

};

// Test 1 from fixture 1
TEST_F(BankAccountTest, BankAccountStartsEmpty)
{
  EXPECT_EQ(0, account->balance);
}

// Test 2 from fixture 1
TEST_F(BankAccountTest, CanDepositMoney)
{
  account->deposit(100);
}

// Fixture 2
struct account_state
{
  int initial_balance;
  int withdraw_amount;
  int final_balance;
  bool success;
  friend std::ostream &operator<<(std::ostream &os,
                                  const account_state &state) {
    os << "initial_balance: " << state.initial_balance
       << " withdraw_amount: " << state.withdraw_amount
       << " final_balance: " << state.final_balance
       << " success: " << state.success;
    return os;
  }
};

// Fixture 2
struct WithdrawAccountTest : BankAccountTest , testing::WithParamInterface<account_state>
{
  WithdrawAccountTest()
  {
    account->balance = GetParam().initial_balance;
  }
};

// Test 1 from fixture 2: Parametarized test
TEST_P(WithdrawAccountTest, FinalBalance)
{
  auto as = GetParam();
  auto success = account->withdraw(as.withdraw_amount);
  EXPECT_EQ(as.final_balance, account->balance);
  EXPECT_EQ(as.success, success);
}

// Test case for P test
INSTANTIATE_TEST_CASE_P(Default, WithdrawAccountTest,
    testing::Values(
        account_state{100,50,50,true},
        account_state{100,200,100,false}
        ));

//--------------------
// other fixture
class IncrementClass
{
  int baseVal;
public:
  IncrementClass(int bv) : baseVal(bv) {}
  void increment(int bv)
  {
    baseVal += bv;
  }
  int getValue() {return baseVal;}
};
struct IncrementFixture : testing::Test
{
  IncrementClass* obj;
  void SetUp()
  {
    std::cout << "SetUp\n";
    obj = new IncrementClass(100);
  }
  void TearDown()
  {
    std::cout << "TearDown\n";
    delete obj;
  }

};
TEST_F(IncrementFixture, inc_5)
{
  obj->increment(5);
  EXPECT_EQ(obj->getValue(), 105);
}
TEST_F(IncrementFixture, inc_10)
{
  obj->increment(10);
  EXPECT_EQ(obj->getValue(), 110);
}

//--Other fixture----
class Stack
{
  std::vector<int> vStack {};

public:
  void push(int value) {vStack.push_back(value);}
  int pop()
  {
    if (!vStack.empty())
    {
      int value = vStack.back();
      vStack.pop_back();
      return value;
    }
    return -1;
  }
  int size() { return  vStack.size(); }
};

struct StackFixture : testing::Test
{
  Stack obj;
  void SetUp() override
  {
    std::array<int, 10> value {1,2,3,4,5,6,7,8,9};
    for (auto& it : value)
    {
      obj.push(it);
    }
  }
  void TearDown() override {}
};

//TEST_F(StackFixture, PopTest)
//{
//  int lastPopVal = 3;
//  while( lastPopVal != 1)
//  {
//    EXPECT_EQ(obj.pop(), --lastPopVal);
//  }
//}
TEST_F(StackFixture, sizeofValid)
{
  int size = obj.size();
  for (size; size > 0; --size)
  {
    ASSERT_NE(obj.pop(), -1);
  }
}

//----------------------------
//-----------gMock------------
//----------------------------
// source
class DataBaseConnect
{
public:
  virtual bool login(std::string username, std::string password)
  {
    return true;
  }
  virtual bool logout(std::string username)
  {
    return true;
  }
  virtual int fetchRecord()
  {
    return -1;
  }
};

class Database
{
private:
  DataBaseConnect& _db;
public:
  explicit Database(DataBaseConnect& db) : _db{db} {}
  int init(std::string username, std::string pwd)
  {
    int result = 0;
    if (_db.login(username, pwd))
    {
      std::cout << "DB_SUCCEEDED\n";
      result = 1;
    }
    else
    {
      std::cout << "DB_FAILED\n";
      result = -1;
    }
    return result;
  }
};

// mock database
class MockDB : public DataBaseConnect
{
public:
  MOCK_METHOD0(fetchRecord, int());
  MOCK_METHOD1(logout, bool (std::string username));
  MOCK_METHOD2(login, bool (std::string usr, std::string pwd) );
};
// test
TEST(DBTest, LoginTest)
{
  MockDB mdb;
  Database db(mdb);

//  EXPECT_CALL(mdb, login("Usr", "Pwd") )
//      .Times(1)
//      .WillOnce(testing::Return(true));
//  EXPECT_CALL(mdb, login("Usr", "Pwd") )
//      .Times(testing::AtLeast(1))
//      .WillOnce(testing::Return(true));
  EXPECT_CALL(mdb, login(testing::_, testing::_))
      .Times(testing::AtLeast(1))
      .WillOnce(testing::Return(true));

  int result = db.init("Usr", "Pwd");

  EXPECT_EQ(result, 1);
}

int main(int argc, char** argv )
{
  std::cout << "Hello, World!" << std::endl;
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
