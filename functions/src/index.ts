import * as functions from "firebase-functions";
import * as admin from "firebase-admin";
// import * as firebaseHelper from "firebase-functions-helper";
import * as express from "express";

import * as bodyParser from "body-parser";

 import {Client} from "@googlemaps/google-maps-services-js";

admin.initializeApp(functions.config().firebase);
const db = admin.firestore();
const app = express();
const main = express();
// const contactsCollection = "users";
main.use("/api/v1", app);
main.use(bodyParser.json());
main.use(bodyParser.urlencoded({ extended: false }));

export const webApi = functions.https.onRequest(main);

// export const GOOGLE_MAPS_API_KEY="AIzaSyDn6ibMkPwRUaMBJwnTLfTyozRCeUaDTUk"

const client = new Client({});

// export const {Client, Status} = require("@google/maps");
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
    policeStation: "none"
  };

  // db.collection("users").doc("0712223331").update(data).then(() => {
  //   console.log('added order');
// }, (error) => {
//     console.error('Failed to add order');
//     console.error(error);
// });

  // console.log(req.params.lat);
  console.log(
    "api request recieved>>" +
      req.params.contactId +
      ":" +
      req.params.long +
      ":" +
      req.params.lat
  );

  setNearestPolice(data,req.params.contactId);
  console.log("all done")
  res.status(200).send(`alert sent`);
});
// AIzaSyDn6ibMkPwRUaMBJwnTLfTyozRCeUaDTUk
//https://maps.googleapis.com/maps/api/place/nearbysearch/json?location=80.086433,7.060751&radius=1000&type=bar&key=AIzaSyDn6ibMkPwRUaMBJwnTLfTyozRCeUaDTUk
// getNearestHostpital();





function setNearestPolice(data:any,contactId:any){
   console.log("getPlaces runs  1")

  try{
  client
  .placesNearby({
    params: {
      location:[data.latitude, data.longitude] ,
      key: "AIzaSyDn6ibMkPwRUaMBJwnTLfTyozRCeUaDTUk",
      radius:20000,
      type: "police",
      
    },
    timeout: 5000, // milliseconds
  })
  .then((r) => {
    console.log("getPlaces success 1")
    console.log(r.data.results[0].name);
    data.policeStation = r.data.results[0].name;
    try {
      db.collection("users").doc(contactId).update(data);
      console.log("db update success")
    } catch (error) {
      console.log(error)
    }



  })
  .catch((e) => {
    console.log(e);
    console.log("getPlaces error  1")

  });
}catch(e){
  console.log(e);
  console.log("getPlaces error  2")

}

}