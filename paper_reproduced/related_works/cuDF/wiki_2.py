import cudf
import time
import pandas as pd


# Measure and print the time taken to read and parse each JSON file
def measure_parsing_time(json_file_path, description, query_version):
    print(f"{description}:\n")
    
    # Measure loading and conversion time
    start_time = time.time()
    df = cudf.read_json(json_file_path)
    end_time = time.time()
    total_time_ms = (end_time - start_time) * 1000
    
    # Calculate the size of the DataFrame in bytes
    df_size_bytes = df.memory_usage(deep=True).sum()

    # Convert size to megabytes
    df_size_mb = df_size_bytes / (1024 * 1024)

    # Print the size in MB
    print(f"DataFrame Size: {df_size_mb:.2f}")
    # # Convert the cuDF DataFrame to a Pandas DataFrame (CPU)
    # start_transfer_time = time.time_ns()
    # df_cpu = df.to_pandas()
    # transfer_time = time.time_ns() - start_transfer_time

    # Display results
    # print(df.head())
    print(f"Time taken to parse: {total_time_ms:.2f} ms\n")
    # print(f"Time taken to transfer data from GPU to CPU: {transfer_time} ns\n")


    # Measure query execution time
    query_start_time = time.time_ns()

    if query_version == 1:
        # Extract specific fields
        first_user_lang = df['user'].iloc[0]['lang'] if 'user' in df and 'lang' in df['user'].iloc[0] else None
        general_lang = df['lang'] if 'lang' in df else None

        # Print extracted values
        # print(f"First user language: {first_user_lang}")
        # print(f"General language: {general_lang}\n")
    elif query_version == 2:
        # Extract specific fields
        first_user_lang = df['user'].iloc[0]['lang'] if 'user' in df and 'lang' in df['user'].iloc[0] else None
        general_lang = df['user'].iloc[0]['id'] if 'lang' in df else None

        # Print extracted values
        # print(f"First user language: {first_user_lang}")
        # print(f"General language: {general_lang}\n")
    elif query_version == 3:
        # Extract the 'id' from the first user's entry
        first_user_id = df['user'].iloc[0]['id'] if 'user' in df and 'id' in df['user'].iloc[0] else None

        # Print extracted value
        # print(f"First user ID: {first_user_id}\n")    
    elif query_version == 4:
        # Extract the first index of the first URL from the first entity
        first_url_index = (df['entities'].iloc[0]['urls'][0]['indices'][0] 
                        if 'entities' in df 
                        and 'urls' in df['entities'].iloc[0] 
                        and 'indices' in df['entities'].iloc[0]['urls'][0] 
                        else None)

        # Print extracted value
        # print(f"First URL Index: {first_url_index}\n")
    elif query_version == 5:
        # Extract the 'price' from the 16th item under 'bestMarketplacePrice'
        price_16th_item = df['bestMarketplacePrice'].iloc[15]['price'] if 'bestMarketplacePrice' in df and len(df) > 15 and 'price' in df['bestMarketplacePrice'].iloc[15] else None
        
        # Extract the 'name' from 'items'
        items_name = df['items']['name'] if 'items' in df and 'name' in df['items'].iloc[0] else None

        # Print extracted values
        # print(f"Price of the 16th item: {price_16th_item}")
        # print(f"Name in items: {items_name}\n")
    elif query_version == 6:
        # Extract the 'descriptions' from the first entry
        descriptions_1st_item = df['descriptions'].iloc[0] if 'descriptions' in df and len(df) > 0 else None
        
        # Print extracted value
        # print(f"Descriptions of the first item: {descriptions_1st_item}\n")
    elif query_version == 7:
        # Extract the 'property' from the second item in 'P1245' under 'claims' of the first entry
        property_value = None
        if 'claims' in df and len(df) > 0:
            claims = df['claims'].iloc[0]
            if 'P1245' in claims and len(claims['P1245']) > 1:
                property_value = claims['P1245'][1]['mainsnak']['property'] if 'mainsnak' in claims['P1245'][1] and 'property' in claims['P1245'][1]['mainsnak'] else None
        
        # Print extracted value
        # print(f"Property value: {property_value}\n")

    elif query_version == 8:
        # Extract the first route
        first_route = df['routes'].iloc[0] if 'routes' in df and len(df) > 0 else None
        
        # Print extracted value
        # print(f"First route: {first_route}\n")    
    elif query_version == 9:
        # Extract the distance text from the first step of the first leg of the first route
        distance_text = None
        if 'routes' in df and len(df) > 0:
            route = df['routes'].iloc[0]
            if 'legs' in route and len(route['legs']) > 0:
                leg = route['legs'][0]
                if 'steps' in leg and len(leg['steps']) > 0:
                    step = leg['steps'][0]
                    if 'distance' in step and 'text' in step['distance']:
                        distance_text = step['distance']['text']
        
        # Print extracted value
        # print(f"Distance text: {distance_text}\n")    
    elif query_version == 10:
        # Extract the regular price of the first product
        regular_price = df['products'].iloc[0]['regularPrice'] if 'products' in df and len(df) > 0 and 'regularPrice' in df['products'].iloc[0] else None
        
        # Print extracted value
        # print(f"Regular price of the first product: {regular_price}\n")


    elif query_version == 11:
        # Extract the IDs from the second and third elements in categoryPath of the first product
        category_ids = []
        if 'products' in df and len(df) > 0:
            product = df['products'].iloc[0]
            if 'categoryPath' in product and len(product['categoryPath']) >= 3:
                category_ids = [category['id'] for category in product['categoryPath'][1:3]]
        
        # Print extracted value
        # print(f"Category IDs of the second and third elements: {category_ids}\n")


    else:
        print("wrong query number!\n")

    query_end_time = time.time_ns()
    query_time = query_end_time - query_start_time
    
    # Print the query execution time
    print(f"Time taken to execute query: {query_time} ns\n")

    

    

# Paths to JSON files
# json_file_path_nspl = '/rhome/aveda002/bigdata/Test-Files/wiki_small_records_remove.json'
# json_file_path_wiki = '/rhome/aveda002/bigdata/Test-Files/wiki_small_records_remove.json'
# json_file_path_walmart = '/rhome/aveda002/bigdata/Test-Files/walmart_small_records_remove.json'
# json_file_path_twitter = '/rhome/aveda002/bigdata/Test-Files/twitter_small_records_remove.json'
# json_file_path_google = '/rhome/aveda002/bigdata/Test-Files/google_map_small_records_remove.json'
# json_file_path_bestbuy = '/rhome/aveda002/bigdata/Test-Files/bestbuy_small_records_remove.json'

# json_file_path_nspl = '../../../dataset/nspl_small_records_remove.json'
json_file_path_wiki = '../../../dataset/wiki_small_records_cudf.json'
# json_file_path_walmart = '../../../dataset/walmart_small_records_remove.json'
# json_file_path_twitter = '../../../dataset/twitter_small_records_remove.json'
# json_file_path_google = '../../../dataset/google_map_small_records_remove.json'
# json_file_path_bestbuy = '../../../dataset/bestbuy_small_records_remove.json'


# Measure and print the parsing times
# measure_parsing_time(json_file_path_wiki, "nspl", 0)

# measure_parsing_time(json_file_path_twitter, "twitter", 1)
# measure_parsing_time(json_file_path_twitter, "twitter", 2)
# measure_parsing_time(json_file_path_twitter, "twitter", 3)
# measure_parsing_time(json_file_path_twitter, "twitter", 4)
# measure_parsing_time(json_file_path_walmart, "walmart", 5)
measure_parsing_time(json_file_path_wiki, "wiki", 6)
# measure_parsing_time(json_file_path_wiki, "wiki", 7)
# measure_parsing_time(json_file_path_google, "google", 8)
# measure_parsing_time(json_file_path_google, "google", 9)
# measure_parsing_time(json_file_path_bestbuy, "bestbuy", 10)
# measure_parsing_time(json_file_path_bestbuy, "bestbuy", 11)
