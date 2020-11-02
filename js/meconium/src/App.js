import React from "react";
import axios from "axios";
import "./styles.css";

export default class App extends React.Component {
  state = {
    message: '',
  };

  componentDidMount() {
    axios.get('/message').then((response) => {
      this.setState({ message: response.data.message });
    });
  }

  render() {
    const { message } = this.state;
    return (
      <div>
      {message}
      </div>
    );
  }
}
