## Q1
- 这里代码需要链接运行在`KERNBASE`之上，但当前的AP仍然处于实模式，只能支持`1MB`的物理地址寻址，所以需要`MPBOOTPHYS`计算相对`MPENTRY_PADDR`即`0X7000`的物理地址，然后才能正确地跳转

---

## Q2
- 即使在`the big kernel lock`存在的情况下，如果只有一个`kernel stack`，那么例如当一个cpu中断发生时，在获取`lock`之前，已经将一些变量push到了stack中，而若有其他的cpu发生中断同时也将一些变量push入stack，那么stack的内容就混乱了。在第一个cpu从stack中读取之前push的数据时，就会出错

---

## Q3
- 用户环境的切换发生在内核中，而内核对不同用户环境的页目录项是映射到相同的地方，是固定的，这里在进行用户环境页表切换后，`curenv`的地址没有改变，映射关系也没有改变，所以没有影响

---

## Q4
- 为了保证下一次切换回来的时候能正常恢复之前的用户环境的运行状态
- 在`trap.c`中，具体为`curenv->env_tf = *tf;`

---

## Challenge
我选择实现第二个challenge，即多优先级调度

- 首先需要在`inc/env.h`中给`Env`结构体增加`priority`属性
- 同时在该文件中定义三个不同的优先级
``` c
#define ENV_PRIO_HIGH 3
#define ENV_PRIO_NOR 2
#define ENV_PRIO_LOW 1
```

- 然后需要在每一个`Env`初始化的时候设置默认的优先级，在`kern/env.c`的`env_alloc()`中添加`e->priority = ENV_PRIO_NOR;`

- 然后需要增加系统调用函数
- 在`inc/syscall.h`中增加`SYS_env_change_priority`

- 在`inc/lib.h`中声明函数`int sys_env_change_priority(envid_t envid, uint32_t priority);`

- 在`lib/syscall.h`中定义函数
```c
int sys_env_change_priority(envid_t envid, uint32_t priority){
    return syscall(SYS_env_change_priority, 0, envid, priority, 0, 0, 0);
}
```

- 在`kern/syscall.c`中定义函数
```c
static int sys_env_change_priority(envid_t envid, uint32_t priority){
    struct Env *env;

    if(envid2env(envid, &env, 1) < 0){
        return -E_BAD_ENV;
    }
    
    env->priority = priority;
    
    return 0;
}
```

- 在`kern/syscall.c`的`syscall`函数中添加分发
``` c
case SYS_env_change_priority:
    return sys_env_change_priority(a1, a2);
```

- 然后需要修改调度算法，现在是每次选取一个优先级最高的执行，在`kern/sched.c`中
```c
if(curenv != NULL){
    int finalID = -1;
    int max_priority = 0;		
    int start = ENVX(curenv->env_id);
    start = (start+1) % NENV;

    i = start;
    do{
        if((envs[i].env_type != ENV_TYPE_IDLE) && (envs[i].env_status== ENV_RUNNABLE) && (envs[i].priority > max_priority)){
            max_priority = envs[i].priority;
            finalID = i;
        }
        i = (i+1) % NENV;
    }while(i != start);	

    if(finalID > -1 && finalID != ENVX(curenv->env_id)){
        env_run(&envs[finalID]);
    }


    if(curenv->env_status == ENV_RUNNING){
        env_run(curenv);	
    }
}
```

#### Test
- 测试时我在`lib/fork.c`中添加了`fork_priority(int priority)`函数，该函数与`fork`的主要区别在于将优先级设置为传入的优先级
    `sys_env_change_priority(thisenv->env_id, priority);`
- 然后修改了`user/hello.c`
```c
int i;
for (i = 3; i >= 1; i--) {
    int re = fork_priority(i);
    if (re == 0) {
        cprintf("get %x \n", i);
        int j;
        for (j = 0; j < 3; j++) {
            cprintf("yielding %x \n", i);
            sys_yield();
        }
        break;
    }
}
```

- 测试的时候运行`make run-hello`，一次测试打印的消息如下
```
get 3 
yielding 3 
get 2 
yielding 2 
yielding 3 
yielding 3 
yielding 2 
[00001009] exiting gracefully
[00001009] free env 00001009
yielding 2 
get 1 
yielding 1 
[0000100a] exiting gracefully
[0000100a] free env 0000100a
yielding 1 
yielding 1 
[0000100b] exiting gracefully
[0000100b] free env 0000100b
No more runnable environments!
```

