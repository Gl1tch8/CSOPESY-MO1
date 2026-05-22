class ClearCommand {
    public:
        ClearCommand(public ClearService clearService) {};

        executeL() {
            clearService.doSomething();
        }

        executeA() {
            clearService.doSomething2();
        }

}

