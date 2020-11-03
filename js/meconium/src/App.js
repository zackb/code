import React from "react";
import "./styles.css";

export default class App extends React.Component {
  state = {
    messages: [],
  };

  componentDidMount() {
    fetch('/message')
      .then(res => res.json())
      .then((data) => 
        this.setState({ messages: data.messages }));
  }

  render() {
    const { messages } = this.state;
    return (
      <div>
        <span>Hello, here are your messages</span>
        {messages.map(msg => <div>{msg.data}</div>)}
      </div>
    );
  }
}
