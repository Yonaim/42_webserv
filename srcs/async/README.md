# async::IOProcessor

> (비)동기사랑 나라사랑

## 구조 설명

모든 `AsyncIOProcessor` 객체는 읽기 버퍼(`_rdbuf`)와 쓰기 버퍼(`_wrbuf`)를 가진다.
전통적 입출력 작업은 입출력 해야하는 데이터가 전부 처리될 때까지 기다려야 하므로 논-블로킹 입출력 작업에 적합하지 않다. 이를 회피하기 위해서는 입출력 하고싶은 데이터를 먼저 버퍼에 등록하고 이들을 블로킹 하지 않는 선에서 허락된 만큼 처리해야 한다.

따라서 비동기 입출력을 위해서는

1. 다른 객체들이 입출력 버퍼에 수시로 데이터를 등록하거나 반출한다.
2. 버퍼의 내용을 블로킹 하지 않는 선에서 처리하는 함수를 호출한다.

위 과정이 무한정 반복되어야 한다.

이때 1번 항목은 다른 클래스가 이 클래스를 사용함으로서 자연스레 구현된다.
하지만 2번 항목은 누군가가 일부러 호출해야한다. 2번 항목의 함수를 `task()` 메소드로 정의하며, `task()` 메소드가 호출되지 않으면 실제 입출력은 일어나지 않는다.
따라서 프로그램이 실행되는 동안 생성된 모든 `IOProcessor` 객체의 `task()`를 번갈아 호출하는 누군가가 필요하다. 이 과정을 용이하게 하기 위해 후술할 `IOTaskHandler` 클래스를 사용한다.

이 클래스는 여러 fd를 처리할 수 있다. 즉 각 fd에 대한 입출력 버퍼를 각각 가지고 있으며 `task()` 메소드를 호출할 때마다 각 fd에 대한 입출력 작업이 동시다발적으로 일어난다. 각 fd를 감시하기 위해 운영체제가 제공하는 시스템 콜을 사용해야 하는데, 이 프로젝트에서는 BSD 커널이 제공하는 `kqueue`(이하 큐)를 사용한다.

### 열거형 값

위에서 2번 항목 (`task()`)이 반환하는 값의 정의는 다음과 같다. 이들은 `JobStatus` 클래스에 정의된다.

- `OK`
  - 입출력 작업이 완료되었음을 의미한다.
- `AGAIN`
  - 입출력 작업이 아직 완료되지 않았으며 본 오브젝트의 `task()`를 다시 호출해줄 것을 요청하는 의미를 가진다.
- `INVALID`
  - 입출력 작업 중 오류가 발생했음을 의미한다. 하지만 대부분의 오류 처리는 `try ... catch`로 이루어질 것이다.

### 멤버 변수의 역할

- `std::map<int, std::string> _rdbuf`
  - 각 fd에 대한 입력 버퍼로서, `task()`를 호출할 때마다 해당 fd에서 읽어온 데이터가 여기에 추가된다.
- `std::map<int, std::string> _wrbuf`
  - 각 fd에 대한 입력 버퍼로서, 여기에 담긴 데이터는 `task()`를 호출할 때마다 가능한 만큼 해당 fd에 출력된다.
- `std::deque<struct kevent> _watchlist`
  - 새로 감시하고 싶은 fd의 목록이 저장된다.
- `std::deque<struct kevent> _eventlist;`
  - 큐에서 반환된 입출력이 가능한 fd의 목록이 저장된다.
- `static bool _debug`
  - 디버그 모드를 활성화한다.

> 프로젝트 특성상 `std::cout`을 사용해 로그를 남겨서는 안 된다. 허나 `IOProcessor`를 사용해 `IOProcessor`의 디버깅 로그를 남기면 무한 루프가 발생하기때문에 한시적으로 `IOProcessor` 내부에서는 `std::cout`을 통해 로그를 남기되 이 동작을 `_debug` 플래그를 통해 제어한다.

### 멤버 함수의 역할

- `void initializeKQueue(void)`
  - 큐를 초기화한다.
- `void flushKQueue(void)`
  - `_watchlist`에 남아있는 새로 감시할 fd의 목록을 큐에 등록하고, 큐에 남아있는 작업 가능한 fd의 목록을 `_eventlist`에 가져온다.
- `void read(const int fd)`
  - 해당 fd에서 가능한 만큼 읽기 작업을 한다. 읽어들인 데이터는 `_rdbuf[fd]`에 추가된다.
- `void write(const int fd)`
  - `_wrbuf[fd]`에 저장된 데이타를 가능한 만큼 해당 fd에 쓴다. 쓰여진 데이터는 버퍼에서 제거된다. 당연히 모든 데이터가 쓰여질 것이라고 보장할 수 없다. 그러한 경우에는 쓰여진 부분만이 버퍼에서 제거된다.
- `void task(void)`
  - 모든 fd에 대해 입출력 작업을 수행한다. `IOTaskHandler::task()`를 호출하면 모든 `IOProcessor`의 `task()`가 호출된다.
- `void blockingWrite(void)`
  - 쓰기 버퍼의 모든 데이터가 쓰여질 때까지 블로킹한다. 프로그램을 종료할 시 사용된다.
- `static void setDebug(bool debug)`
  - 디버그 상태를 제어한다.

### 예외

- `IOProcessor::FileClosed`
- `IOProcessor::ReadError`
- `IOProcessor::WriteError`

# async::IOTaskHandler

이 클래스는 싱글톤 패턴을 따르며 모든 메소드가 정적이다(사실상 단일한 전역 오브젝트처럼 사용한다).

`IOProcessor` 오브젝트가 하나 생성될 때마다 이 클래스에 자신의 주소를 등록한다. 반대로 소멸될 때는 자신의 주소를 등록 해제한다.

이 클래스도 `task()`와 `blockingWrite()` 메소드를 가지고 있으며, 이들은 단순히 여태 등록된 모든 `IOProcessor` 오브젝트의 상응하는 메소드를 호출한다. 따라서 `task()`는 프로그램이 최대한 자주 호출해야 하며 프로그램이 종료되기 전에는 `blockingWrite()`를 호출함이 바람직하다.

# async::SingleIOProcessor

이 클래스는 `IOProcessor`의 기능을 계승하되, 단 하나의 fd만을 처리할 수 있다.
따라서 이 클래스는 표준 입력이나 inode같은 파일에 비동기 입출력을 하는 데 유용하다.

### 멤버 함수의 역할

- `void setWriteBuf(const std::string &str)`
  - `str`의 내용을 출력 버퍼에 추가한다.
- `void getReadBuf(std::string &str)`
  - 입력 버퍼에 있던 내용을 `str`에 추가한다.
- `bool writeDone(void)`
  - 출력 작업이 다 되었는지 반환한다. 즉 출력 버퍼가 비었는지 여부를 반환한다.

### 특징

이 클래스는 `<<`와 `>>` 연산자를 통해 마치 `std::stream` 객체처럼 사용할 수 있다.
따라서 다음 구문이 유효하다.

```cpp
async::SingleIOProcessor cout(STDOUT_FILENO);
cout << "Hello, World!\n";
async::IOTaskHandler::`task()`;
```

자주 사용되는 표준 입력/출력/오류 fd에 대해 전역 오브젝트가 마련되어 있다.

```cpp
async::cout << "Hello, World!\n";
async::cerr << "Hello, Error!\n";
std::string response;
async::cin >> response;
async::IOTaskHandler::`task()`;
```

이때 `std::endl`에 상응하는 기능은 마련되어 있지 않다. `endl`은 입출력 작업이 완료될 때까지 블로킹하는 것을 의미하므로 본 기능에 적합하지 않다. 따라서 호출자는 매번 개행 문자를 삽입해야 한다.

# async::FileIOProcessor

이 클래스는 `SingleIOProcessor` 클래스를 계승하며, 한 파일(fd)에 대해 입출력 작업을 하는데, 생성시 시간 제한을 명시해야 하며 입출력 작업이 완료되었는지 확인할 수 있다.

이 클래스는 추상 클래스이며 하단의 두 클래스는 오브젝트 생성이 가능하다.

이 클래스는 목표 fd를 직접 지정할 수도 있고, 목표 파일의 경로를 지정할 수도 있다. 이때 목표 fd를 지정했다면 이 오브젝트가 소멸될 시 파일을 닫지 않는다. 목표 파일의 경로를 지정할 시 해당 경로의 파일을 직접 열어 fd를 획득하고, 오브젝트 소멸 시 fd를 닫는다.

### 멤버 함수의 역할

- `int task(void)`
  - 논-블로킹 입출력 작업을 수행하고 작업 완료시 `LOAD_STATUS_OK`를 반환한다.
- `std::string retrieve(void)`
  - 작업 결과를 불러온다. 아직 작업이 완료되지 않았을 시 에러를 `throw`한다.

## async::FileWriter

이 클래스는 특정 파일에 데이터를 출력하는 역할을 한다.

### 생성자

- `FileWriter(unsigned int timeout_ms, int fd, const std::string &content)`
  - `timeout_ms`: 시간 제한을 밀리초 단위로 지정
  - `fd`: 파일의 지정자 지정
  - `content`: 파일에 쓸 데이터 지정

- `FileWriter(unsigned int timeout_ms, const std::string &path, const std::string &content)`
  - `timeout_ms`: 시간 제한을 밀리초 단위로 지정
  - `path`: 파일의 경로 지정
  - `content`: 파일에 쓸 데이터 지정

## async::FileReader

이 클래스는 특정 파일에서 데이터를 불러오는 역할을 한다. 특성은 위 클래스들과 동일하다.

# async::TCPIOProcessor

이 클래스는 다수의 TCP 연결을 관리한다.
이 클래스의 특징은 `task()`를 호출할 때마다 새로 접속을 요청하는 클라이언트의 fd가 버퍼에 추가되고 연결이 해제된 fd는 버퍼에서 삭제된다는 점이다. 따라서 fd의 목록이 `task()`를 호출할 때마다 변할 수 있고, 현재 fd의 목록을 호출자에게 알려주는 과정이 복잡해질 수 있다. 이 클래스는 이를 반복자를 사용해 해결한다.

### 멤버 변수의 역할

- `static const int _backlog`
  - 새로 연결되는 클라이언트가 대기할 수 있는 큐의 크기를 정의한다.
- `int _port`
  - 이 오브젝트의 포트 번호다.
- `int _listening_socket`
  - `listen()`을 수행하는 소켓의 fd다.

### 멤버 함수의 역할

- `void accept(void)`
  - 현재 대기중인 클라이언트 중 맨 앞 클라이언트와 연결을 수립하고 해당 fd를 버퍼에 추가한다.
- `void disconnect(const int client_socket)`
  - 클라이언트 연결을 해제한다.
- `void initialize(void)`
  - TCP 연결 대기를 시작한다. 이 메소드는 외부에서 직접 호출해야한다.
    - 이 오브젝트를 컨테이너에 넣을 시 생성자 또는 할당 연산자가 자동으로 호출되는데, 이때 `initialize()`를 자동으로 호출하면 소켓 연결에 혼선이 생길 수 있기 때문에 이 메소드를 외부에서 각각의 유일한 `TCPIOProcessor` 오브젝트마다 호출해줘야 한다.
- `void finalize(const char *with_error)`
  - TCP 연결 대기를 종료한다.
- `virtual void task(void)`
  - 모든 입출력 버퍼에 대해 클라이언트와 통신하고 새로운 연결을 수립하며 통신이 종료된 클라이언트를 연결 해제한다.
- `std::string &rdbuf(const int fd)`
  - 해당 fd(클라이언트)에 대한 입력 버퍼를 가져온다.
- `std::string &wrbuf(const int fd)`
  - 해당 fd(클라이언트)에 대한 출력 버퍼를 가져온다.

## fd 반복자

이 반복자는 `TCPIOProcessor` 객체가 현재 가진 fd(클라이언트)의 목록을 외부에 간결하게 전달하기 위해 고안되었다.

```cpp
async::TCPIOProcessor tcp_proc;
for (async::TCPIOProcessor::iterator it = tcp_proc.begin(); it != tcp_proc.end(); it++)
{
    int fd = *it;
}
```

위 구문을 통해 현재 `TCPIOProcessor`가 보유중인 fd의 목록에 순차적으로 접근할 수 있다. 이 반복자는 `task()`를 호출하면 더 이상 유효하지 않게 된다.
