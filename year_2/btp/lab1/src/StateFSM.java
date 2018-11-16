public class StateFSM extends FSM{
    private State state;

    public StateFSM()
    {
        state = new Q0();
    }

    public void setState(State state) {
        this.state = state;
    }

    public State getState() {
        return state;
    }

    public void start() {
        state = new Q0();
    }

    protected States nextState(Events curEvent) {
        state.doAction(this, curEvent);
        return States.valueOf(state.toString());
    }
}

