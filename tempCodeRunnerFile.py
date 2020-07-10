
query_result2 = google_places.get_place(

   place_id = placeID

 )

if query_result2.has_attributions: 
    print (query_result2.html_attributions) 