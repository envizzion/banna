import * as functions from "firebase-functions";
import * as admin from "firebase-admin";
// import * as firebaseHelper from "firebase-functions-helper";
import * as express from "express";

import * as bodyParser from "body-parser";

admin.initializeApp(functions.config().firebase);
const db = admin.firestore();
const app = express();
const main = express();
// const contactsCollection = "users";
main.use("/api/v1", app);
main.use(bodyParser.json());
main.use(bodyParser.urlencoded({ extended: false }));

export const webApi = functions.https.onRequest(main);

app.get("/locations/:contactId&:long&:lat", (req, res) => {
  // firebaseHelper.firestore.updateDocument(
  //   db,
  //   contactsCollection,
  //   req.params.contactId,
  //   {
  //     longitude: req.params.long,
  //     latitude: req.params.lat,
  //     inDanger: true,
  //   });
  let data = {
    longitude: req.params.long,
    latitude: req.params.lat,
    inDanger: true,
  };

  db.collection("users").doc(req.params.contactId).update(data);

  // console.log(req.params.lat);
  console.log(
    "api request recieved>>" +
      req.params.contactId +
      ":" +
      req.params.long +
      ":" +
      req.params.lat
  );
  res.status(200).send(`alert sent`);
});
