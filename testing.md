# Browser acceptance testing Guide
  
### Installation
  - Download requestly for chrome at https://chrome.google.com/webstore/detail/requestly-redirect-url-mo/mdnleldcmiljblolnjhpnblkcekpdkpa or for firefox at 
   
  - Navigate to https://app.requestly.io/rules/ and create a new Rule, select Redirect Request
  
  - Add all sites you wish to browse through the jimber browser.
   
  -  Use following example format : 
   
 **Request URL Matches** \*reddit.com\*

 **Destination URL** https://browser.staging.jimber.org/#$1reddit.com$2

  - Add as many sites as you want following this example, using the +NEW button

    ![Requestly Rule Example](https://i.imgur.com/QlW9Byh.png)

  - Make sure the rule is turned on when you want to test ! 