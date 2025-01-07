// for page navigation & to sort on leftbar

export type EachRoute = {
  title: string;
  href: string;
  noLink?: true;
  items?: EachRoute[];
};

export const ROUTES: EachRoute[] = [
  {
    title: "Design Document",
    href: "/designdoc",
    noLink: true,
    items: [
      { 
        title: "Introduction",
        href: "/introduction",
        noLink : true ,
        items : [
          { title: "Overview", href: "/purpose" },
          { title: "Background", href: "/bg" },
          // { title: "Objective", href: "/obj" }, /dk if this is needed 
        ]
      },
      {
        title : "System Overview",
        href  : "/sysover",
        noLink: true ,
        items : [
          { title: "System Level Design", href: "/arch" },
          { title: "Functional Requirements", href: "/funcreq" },
          { title: "Non-Functional Requirements", href: "/nfuncreq" },
        ],
      }
    ],
  },
  {
    title: "Setting Up Bluetooth Server",
    href: "/bluetooth_server",
    noLink: true,  // Parent item, no direct link
    items: [
      { title: "Setting Up Non-Volatile Storage", href: "/nvs"},  
      { title: "Setting Up Bluetooth Controller", href: "/bts" },  
      { title: "Application Profiles", href: "/profiles" },  
      { title: "GAP Parameters", href: "/gapp" },  
    ]
  },
  {
    title: "Bluetooth Communication",
    href: "/bluetooth_comm",
    noLink: true,  // Parent item, no direct link
    items: [
      { title: "Reading Data", href: "/creads"},  
    ]
  },
];

type Page = { title: string; href: string };

function getRecurrsiveAllLinks(node: EachRoute) {
  const ans: Page[] = [];
  if (!node.noLink) {
    ans.push({ title: node.title, href: node.href });
  }
  node.items?.forEach((subNode) => {
    const temp = { ...subNode, href: `${node.href}${subNode.href}` };
    ans.push(...getRecurrsiveAllLinks(temp));
  });
  return ans;
}

export const page_routes = ROUTES.map((it) => getRecurrsiveAllLinks(it)).flat();
