# Design Patterns

A collection of design patterns implemented in C.

## Creational Patterns (创建型)

| Pattern | Description | Status |
|---------|-------------|--------|
| [Singleton](./singleton/) | 确保一个类只有一个实例 | TODO |
| [Factory](./factory/) | 定义创建对象的接口，让子类决定实例化哪个类 | TODO |
| [Abstract Factory](./abstract-factory/) | 创建一系列相关或依赖对象的接口 | TODO |
| [Builder](./builder/) | 将复杂对象的构建与其表示分离 | TODO |
| [Prototype](./prototype/) | 通过复制现有对象来创建新对象 | TODO |

## Structural Patterns (结构型)

| Pattern | Description | Status |
|---------|-------------|--------|
| [Facade](./facade_demo/) | 为子系统提供统一的高层接口 | ✓ |
| [Adapter](./adapter/) | 将一个类的接口转换成客户期望的接口 | TODO |
| [Decorator](./decorator/) | 动态地给对象添加职责 | TODO |
| [Proxy](./proxy/) | 为其他对象提供代理以控制访问 | TODO |
| [Composite](./composite/) | 将对象组合成树形结构 | TODO |
| [Bridge](./bridge/) | 将抽象与实现分离 | TODO |
| [Flyweight](./flyweight/) | 共享细粒度对象以节省内存 | TODO |

## Behavioral Patterns (行为型)

| Pattern | Description | Status |
|---------|-------------|--------|
| [Observer](./observer/) | 定义对象间的一对多依赖关系 | TODO |
| [Strategy](./strategy/) | 定义算法族，使它们可以互换 | TODO |
| [Command](./command/) | 将请求封装为对象 | TODO |
| [State](./state/) | 允许对象在状态改变时改变行为 | TODO |
| [Template Method](./template-method/) | 定义算法骨架，延迟某些步骤到子类 | TODO |
| [Iterator](./iterator/) | 顺序访问集合元素而不暴露其表示 | TODO |
| [Mediator](./mediator/) | 用中介对象封装一系列对象交互 | TODO |
| [Chain of Responsibility](./chain/) | 使多个对象都有机会处理请求 | TODO |

## Build

Each pattern is an independent project. See individual README for build instructions.

```bash
# Example: Build facade pattern
cd facade_demo
cmake -S . -B build
cmake --build build
./build/facade_demo.exe
```

## Learning Path

Recommended order to learn design patterns:

1. **Singleton** - Simplest pattern, good starting point
2. **Factory** - Common in real projects
3. **Facade** - Simplifies complex systems
4. **Observer** - Event-driven programming
5. **Strategy** - Algorithm flexibility
6. **Decorator** - Extending behavior without inheritance
7. **Adapter** - Interface compatibility
8. **Command** - Undo/redo, queuing
