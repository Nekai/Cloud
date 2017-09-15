# Autopopulate ~/.ssh/authorized_keys

Place external authorized **public** keys *(yes, public! why would you
want your private keys on Github?)* here as individual files in the
per-user subdirectories:

```
   + user1/
   |      +--- key1.pub
   |      +--- key2.pub
   |
   + user2/
          +--- key3.pub
```

Script `files/012_add_authorized_keys.sh` will append them to corresponding
~user/.ssh/authorized_keys for everyone who wants them autopopulated.
